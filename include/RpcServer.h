#include "esp_http_server.h"
#include <ArduinoJson.h>

struct rpc_request{
    public:
        rpc_request(httpd_req_t *httpdReq);

        template<typename T>
        bool TryGetParam(const char* key, T& out);

        ~rpc_request();
    private:
        httpd_req_t *httpdReq;
        char *queryStrBuf;
};

using rpc_handler_t = esp_err_t (*)(rpc_request* req, char* resp, size_t len);

template <typename T>
using rpc_get_handler_t = T (*)();

template <typename T>
using rpc_set_handler_t = esp_err_t (*)(T val);

class RpcServer{
    public:
        RpcServer(const char *uri = "/rpc/");
        esp_err_t Start();
        void Stop();

        esp_err_t RegisterHandler(const char *uri, httpd_method_t method, rpc_handler_t handler);

        /**
         * @brief Registers a GET handler
         * 
         * @tparam T Type of the getter return value
         * @param uri RPC endpoint I.E. Name of the value to GET
         * @param getter Function that returns the value to GET
         * @return esp_err_t ESP_OK on successful registration
         */
        template <typename T>
        esp_err_t RegisterGetHandler(const char *uri, T (*getter)()){

            int len = strlen(this->uri) + strlen(uri);
            char ep[len + 1];

            strcpy(ep, this->uri);
            strcat(ep, uri);
            
            httpd_uri_t cfg = {
                .uri = ep,
                .method = HTTP_GET,
                .handler = HandleGet<T>,
                .user_ctx = (void *)getter
            };

            httpd_register_uri_handler(server, &cfg);

            return ESP_OK;
        }

        /**
         * @brief Registers a SET handler
         * 
         * @tparam T The type of the value being SET
         * @param uri RPC endpoint I.E. Name of the value to SET
         * @param setter Function that SETs the value
         * @return esp_err_t ESP_OK on successful registration
         */
        template <typename T>
        esp_err_t RegisterSetHandler(const char *uri, void (*setter)(T val)){
            int len = strlen(this->uri) + strlen(uri);
            char ep[len + 1];

            strcpy(ep, this->uri);
            strcat(ep, uri);

            httpd_uri_t cfg = {
                .uri = ep,
                .method = HTTP_PUT,
                .handler = HandleSet<T>,
                .user_ctx = (void *)setter
            };

            httpd_register_uri_handler(server, &cfg);

            return ESP_OK;
        }

    private:
        httpd_handle_t server;
        const char *uri;

        static esp_err_t Handle(httpd_req_t* req);

        /**
         * @brief Handles GET requests. This is needed because the httpd api
         * requires a callback in the global scope
         * 
         * @tparam T The type of the value to GET
         * @param req The http request
         * @return esp_err_t ESP_OK
         */
        template <typename T>
        static esp_err_t HandleGet(httpd_req_t *req){
            auto getter = (rpc_get_handler_t<T>)req->user_ctx;
            auto parsed = String(getter());

            httpd_resp_send(req, parsed.c_str(), HTTPD_RESP_USE_STRLEN);
            return ESP_OK;
        }

        /**
         * @brief Handles SET (HTTP_PUT) requests by performing validation and parsing then passing the result to the setter function.
         * @tparam T The type of the value to SET. Validation is performed based on this parameter
         * @param req The http request
         * @return esp_err_t 
         *  - ESP_OK if http request content satisfies validation of the given type.
         *  - ESP_ERR_INVALID_ARG if the http request content does not pass validation of the given type
         * @details This template should have a specialization for each supported type as validation should be
         * performed here to ensure the data received is valid before calling the setter. This is static because the httpd api
         * requires a callback in the global scope.
         */
        template <typename T>
        static esp_err_t HandleSet(httpd_req_t *req);
};