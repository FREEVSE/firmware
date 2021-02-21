#include <rpc.h>
#include <Configuration.h>

//httpd_handle_t RpcServer::server;

template <class Tpayload>
RpcServer<Tpayload>::RpcServer(const char *uri, MessageParser<Tpayload> parser = NULL) : parser(parser){
    this->uri = uri;
    this->parser = parser;
}

template <class Tpayload>
esp_err_t RpcServer<Tpayload>::Start(){
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();
    return httpd_start(&server, &config);
}

template <class Tpayload>
void RpcServer<Tpayload>::Stop(){
    if (server) {
        httpd_stop(server);
    }
}

template <class Tpayload>
esp_err_t RpcServer<Tpayload>::RegisterHandler(const char *uri, httpd_method_t method, rpc_handler_t<Tpayload> handler){
    int len = strlen(this->uri) + strlen(uri);
    char ep[len + 1];
    
    strcpy(ep, this->uri);
    strcat(ep, uri);

    httpd_uri_t cfg = {
        .uri = ep,
        .method = method,
        .handler = HandleRpc<Tpayload>,
        .user_ctx = this
    };
}

template <class Tmsg>
esp_err_t HandleRpc(httpd_req_t *req){
    auto server = (RpcServer<Tmsg> *)req->user_ctx;

    if (this->parser != NULL)
    {
        char buf[req.content_len];

        httpd_req_recv(req, buf, req.content_len);
        auto msg = this->parser.Deserialize(buf);
        auto reply = handler(msg);
    }
}