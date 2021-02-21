#include <MessageParser.h>
#include "esp_http_server.h"

template <class Tpayload>
using rpc_handler_t = Tpayload (*)(Tpayload);

template <class Tmsg>
class RpcServer{
    public:
        RpcServer(const char *uri = "/rpc/", MessageParser<Tmsg> parser = NULL);
        const MessageParser<Tmsg> parser;
        esp_err_t Start();
        void Stop();
        esp_err_t RegisterHandler(const char *uri, httpd_method_t method, rpc_handler_t<Tmsg> handler);

    private:
        httpd_handle_t server;
        const char *uri;
        esp_err_t HandleRpc(httpd_req_t *req);
};