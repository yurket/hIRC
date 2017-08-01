FROM ubuntu

WORKDIR /hIRC
ADD ./build/bin/ /hIRC
ENTRYPOINT ["./ircHistoryBot"]
