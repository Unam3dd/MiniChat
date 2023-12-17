FROM alpine:3.19.0
RUN apk update && apk upgrade && apk add --no-cache make wine gstreamer bc git sudo
RUN wine64 --version && wine64 winecfg
RUN addgroup -S windows && adduser -S dev -G windows
RUN echo "dev ALL=(ALL) ALL" > /etc/sudoers.d/dev && chmod 0440 /etc/sudoers.d/dev
USER dev
ENV HOME /home/dev/
STOPSIGNAL SIGTERM
ENTRYPOINT ["/bin/sh"]
