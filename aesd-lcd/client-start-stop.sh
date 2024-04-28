#! /bin/sh

#script file which uses start-stop-daemon to start the client application in daemon mode with the -d option
#sends SIGTERM to gracefully exit when stopped
#Starts with -S (if the daemon doesn’t exist)
#Sends SIGTERM with -K

case "$1" in
    start)
        echo "Starting aesdsocket client"
	start-stop-daemon -S -n client -a /usr/bin/aesdsocket -- -d
        ;;
    stop)
        echo "Stopping aesdsocket client"
        start-stop-daemon -K -n client
        ;;
    *)
        echo "Usage: $0 {start|stop}"
    exit 1
esac

exit 0
