enum vc_signals{INITIAL_SIGNAL,WAIT_FOR_PARTNER,PARTNER_CONNECTED,SEND_END_CALL,END_CALL };

struct signalling
{
    int video_port;
    int audio_port;
    char username[20];
    enum vc_signals sig;
};