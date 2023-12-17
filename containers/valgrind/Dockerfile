FROM debian:stable-slim
RUN apt-get update && apt-get upgrade -y && apt-get install gcc make zsh bc valgrind -y && apt-get clean
STOPSIGNAL SIGTERM
ENTRYPOINT ["/usr/bin/valgrind", "--leak-check=full", "--track-fds=yes", "--show-leak-kinds=all", "--track-origins=yes"]
