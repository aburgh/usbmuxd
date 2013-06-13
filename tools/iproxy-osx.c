#include <asl.h>
#include <assert.h>
#include <errno.h>
#include <launch.h>
#include <netdb.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>			// EXIT_*
#include <unistd.h>			// close
#include <sys/event.h>		// kqueue
#include "sock_stuff.h"
#include "usbmuxd.h"

/* TODO:
 *   - read arguments for ports settings to allow direct execution (and add option for launchd mode)
 *   - add option for targeting a specific udid
 *   - fix recv error when closing connects, or see below
 *   - use kevent for stoc and ctos loops, condensing into one function (or use GCD)
 *   - add connection count tracking and exit when 0
 */

int kq;
aslclient asl = NULL;
aslmsg log_msg = NULL;

struct launch_context {
	int retval;
};

struct client_data {
    int fd;
    int sfd;
    volatile int stop_ctos;
    volatile int stop_stoc;
};

void *run_stoc_loop(void *arg)
{
	// This thread is subordinate to run_ctos_loop, so it doesn't
	// clean up 
    struct client_data *cdata = (struct client_data *) arg;
    int recv_len;
    int sent;
    char buffer[131072];

    printf("%s: fd = %d\n", __func__, cdata->fd);

    while (!cdata->stop_stoc && cdata->fd > 0 && cdata->sfd > 0) {
		
		recv_len = recv_buf_timeout(cdata->sfd, buffer, sizeof(buffer), 0, 5000);
		if (recv_len <= 0) {
			if (recv_len == 0) {
				// try again
				continue;
			} else {
				asl_log(asl, log_msg, ASL_LEVEL_ERR, "recv failed: (%d) %m", errno);
				break;
			}
		} else {
			//	    printf("received %d bytes from server\n", recv_len);
			// send to socket
			sent = send_buf(cdata->fd, buffer, recv_len);
			if (sent < recv_len) {
				if (sent <= 0) {
					asl_log(asl, log_msg, ASL_LEVEL_ERR, "send failed: (%d) %m\n", errno);
					break;
				} else {
					asl_log(asl, log_msg, ASL_LEVEL_WARNING, "only sent %d from %d bytes\n", sent, recv_len);
				}
			} else {
				// sending succeeded, receive from device
				//		printf("pushed %d bytes to client\n", sent);
			}
		}
    }
    cdata->stop_ctos = 1;

    return NULL;
}

void *run_ctos_loop(void *arg)
{
    struct client_data *cdata = (struct client_data *) arg;
    int recv_len;
    int sent;
    char buffer[131072];
    pthread_t stoc;

	assert(cdata->fd > 0 && cdata->sfd > 0);
	
    asl_log(asl, log_msg, ASL_LEVEL_DEBUG, "%s: fd = %d\n", __func__, cdata->fd);

    pthread_create(&stoc, NULL, run_stoc_loop, cdata);

    while (!cdata->stop_ctos && cdata->fd > 0 && cdata->sfd > 0) {
		
		recv_len = recv_buf_timeout(cdata->fd, buffer, sizeof(buffer), 0, 5000);
		if (recv_len <= 0) {
			if (recv_len == 0) {
				// try again
				continue;
			} else {
				asl_log(asl, log_msg, ASL_LEVEL_ERR, "recv failed: (%d) %m", errno);
				break;
			}
		} else {
			//	    printf("pulled %d bytes from client\n", recv_len);
			// send to local socket
			sent = send_buf(cdata->sfd, buffer, recv_len);
			if (sent < recv_len) {
				if (sent <= 0) {
					asl_log(asl, log_msg, ASL_LEVEL_ERR, "send failed: (%d) %m", errno);
					break;
				} else {
					asl_log(asl, log_msg, ASL_LEVEL_ERR, "only sent %d from %d bytes", sent, recv_len);
				}
			} else {
				// sending succeeded, receive from device
				asl_log(asl, log_msg, ASL_LEVEL_DEBUG, "sent %d bytes to server", sent);
			}
		}
    }
    cdata->stop_stoc = 1;
    pthread_join(stoc, NULL);

    close(cdata->fd);
    cdata->fd = -1;

	usbmuxd_disconnect(cdata->sfd);
	free(cdata);
	
    return NULL;
}

int run_kevent_loop(void)
{
	struct kevent kev;
	struct sockaddr_storage ss;
	socklen_t slen = sizeof(ss);
//	FILE *the_stream;
//	int filedesc;

	struct client_data *cdata;
	pthread_t ctos;
	int count;
	int fd, sfd, device_port;

	for (;;) {

		usbmuxd_device_info_t *dev_list = NULL;

		//
		// Get the next event from the kernel event queue.
		//
		if (kevent(kq, NULL, 0, &kev, 1, NULL) == -1) {
//			if (errno == EINTR)
//				continue;
			asl_log(asl, log_msg, ASL_LEVEL_ERR, "kevent(): (%d) %m", errno);
			return EXIT_FAILURE;
		}
//		else if (filedesc == 0) {
//			return EXIT_SUCCESS;
//		}

		device_port = atoi(kev.udata);

		//
		// Accept an incoming connection.
		//
		if ((fd = accept(kev.ident, (struct sockaddr *)&ss, &slen)) == -1) {
			asl_log(asl, log_msg, ASL_LEVEL_ERR, "accept(): (%d) %m", errno);
			continue; /* this isn't fatal */
		}

		asl_log(asl, log_msg, ASL_LEVEL_NOTICE, "Connection accepted: %s", kev.udata);

		if ((count = usbmuxd_get_device_list(&dev_list)) < 0) {
			asl_log(asl, log_msg, ASL_LEVEL_WARNING, "Requesting device list failed.");
			free(dev_list);
			close(fd);
			continue;
		}

		asl_log(asl, log_msg, ASL_LEVEL_INFO, "Number of available devices == %d\n", count);

		if (dev_list == NULL || dev_list[0].handle == 0) {
			asl_log(asl, log_msg, ASL_LEVEL_WARNING, "No connected device found.");
			free(dev_list);
			close(fd);
			continue;
		}

		asl_log(asl, log_msg, ASL_LEVEL_NOTICE, "Requesting connection to device handle == %d (serial: %s), port %d", dev_list[0].handle, dev_list[0].udid, device_port);

		sfd = usbmuxd_connect(dev_list[0].handle, device_port);
		free(dev_list);

		if (sfd < 0) {
			asl_log(asl, log_msg, ASL_LEVEL_ERR, "usbmuxd_connect(): (%d) %m", errno);
			close(fd);
		} else {
			cdata = calloc(1, sizeof(struct client_data));
			cdata->fd = fd;
			cdata->sfd = sfd;
			
			pthread_create(&ctos, NULL, run_ctos_loop, cdata);
		}
	}
}

void register_socket_entry(const launch_data_t listening_fd_array, const char *key, void *context)
{
	struct launch_context *ctx = context;
	char *prefix = "Remote Port:";
	char *akey;
	launch_data_t fd;
	struct kevent kev;

	if ((key = strstr(key, prefix)) == NULL)
		return;
	key += strlen(prefix);
	akey = malloc(strlen(key) + 1);
	strcpy(akey, key);

	for (int i = 0; i < launch_data_array_get_count(listening_fd_array); i++) {

		fd = launch_data_array_get_index(listening_fd_array, i);

		EV_SET(&kev, launch_data_get_fd(fd), EVFILT_READ, EV_ADD, 0, 0, akey);
		if (kevent(kq, &kev, 1, NULL, 0, NULL) == -1) {
			asl_log(asl, log_msg, ASL_LEVEL_DEBUG, "kevent(): %m");
			ctx->retval = EXIT_FAILURE;
			return;
		}
	}
}

int launchd_checkin(void)
{
	struct launch_context ctx;
	launch_data_t sockets_dict, checkin_request, checkin_response;

	// Register ourselves with launchd.
	//
	if ((checkin_request = launch_data_new_string(LAUNCH_KEY_CHECKIN)) == NULL) {
		asl_log(asl, log_msg, ASL_LEVEL_ERR, "launch_data_new_string(\"" LAUNCH_KEY_CHECKIN "\") Unable to create string.");
		return EXIT_FAILURE;
	}

	if ((checkin_response = launch_msg(checkin_request)) == NULL) {
		asl_log(asl, log_msg, ASL_LEVEL_ERR, "launch_msg(\"" LAUNCH_KEY_CHECKIN "\") IPC failure: %m");
		return EXIT_FAILURE;
	}

	if (LAUNCH_DATA_ERRNO == launch_data_get_type(checkin_response)) {
		errno = launch_data_get_errno(checkin_response);
		asl_log(asl, log_msg, ASL_LEVEL_ERR, "Check-in failed: %m");
		return EXIT_FAILURE;
	}

	launch_data_t the_label = launch_data_dict_lookup(checkin_response, LAUNCH_JOBKEY_LABEL);
	if (NULL == the_label) {
		asl_log(asl, log_msg, ASL_LEVEL_ERR, "No label found");
		return EXIT_FAILURE;
	}
	asl_log(asl, log_msg, ASL_LEVEL_NOTICE, "Label: %s", launch_data_get_string(the_label));

	// Retrieve the dictionary of Socket entries in the config file

	sockets_dict = launch_data_dict_lookup(checkin_response, LAUNCH_JOBKEY_SOCKETS); // "Sockets"
	if (NULL == sockets_dict || launch_data_dict_get_count(sockets_dict) == 0) {
		asl_log(asl, log_msg, ASL_LEVEL_ERR, "No sockets found to answer requests on!");
		return EXIT_FAILURE;
	}

	launch_data_dict_iterate(sockets_dict, register_socket_entry, &ctx);

	launch_data_free(checkin_response);

	return EXIT_SUCCESS;
}

int main(int argc, char *argv[])
{
	int retval = EXIT_SUCCESS;

	// Create a new ASL log
	//
	asl = asl_open("iproxy-osx", "Daemon", ASL_OPT_STDERR);
	log_msg = asl_new(ASL_TYPE_MSG);
	asl_set(log_msg, ASL_KEY_SENDER, "iproxy-osx");

	// Note the use of the '%m' formatting character. ASL will replace %m with the error string associated
	// with the current value of errno.
	//
	if ((kq = kqueue()) == -1) {
		asl_log(asl, log_msg, ASL_LEVEL_ERR, "kqueue(): %m");
		retval = EXIT_FAILURE;
		goto done;
	}

	if ((retval = launchd_checkin()) != EXIT_SUCCESS)
		goto done;

	retval = run_kevent_loop();
	
done:
	asl_close(asl);
	return retval;
}