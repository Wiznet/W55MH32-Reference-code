#ifndef _WEBPAGE_H_
#define _WEBPAGE_H_

/*************************************************************************************
 * HTML Pages (web pages)
 *************************************************************************************/

#define index_page "<html>\n"                                                                                                                                                                            \
                   "<head>\n"                                                                                                                                                                            \
                   "    <title>W55MH32 Configuration Page</title>\n"                                                                                                                                     \
                   "    <style>\n"                                                                                                                                                                       \
                   "        body { font-family: Arial, sans-serif; margin: 20px; padding: 20px; background-color: #f4f4f9; }\n"                                                                          \
                   "        h1 { text-align: center; color: #333; }\n"                                                                                                                                   \
                   "        form { max-width: 400px; margin: auto; background: #ffffff; padding: 20px; border-radius: 8px; box-shadow: 0 0 10px rgba(0, 0, 0, 0.1); }\n"                                 \
                   "        label { display: block; margin-bottom: 8px; font-weight: bold; }\n"                                                                                                          \
                   "        input[type=text], input[type=submit] { width: 400px; padding: 8px; margin-bottom: 12px; border: 1px solid #ccc; border-radius: 4px; }\n"                                     \
                   "        input[type=submit] { background-color: #4CAF50; color: white; border: none; cursor: pointer; }\n"                                                                            \
                   "        input[type=submit]:hover { background-color: #45a049; }\n"                                                                                                                   \
                   "        input[readonly] { background-color: #e9ecef; color: #6c757d; border: 1px solid #ced4da; cursor: not-allowed; }\n"                                                            \
                   "        .error { color: red; font-size: 12px; margin-top: -10px; margin-bottom: 10px; }\n"                                                                                           \
                   "        input::placeholder { color: #aaa; font-style: italic; }\n"                                                                                                                   \
                   "    </style>\n"                                                                                                                                                                      \
                   "    <script>\n"                                                                                                                                                                      \
                   "        function validateForm(event) {\n"                                                                                                                                            \
                   "            const ipPattern = /^(\\d{1,3}\\.){3}\\d{1,3}$/;\n"                                                                                                                       \
                   "            let isValid = true;\n"                                                                                                                                                   \
                   "            document.querySelectorAll('.error').forEach(e => e.textContent = '');\n"                                                                                                 \
                   "            const fields = ['ip', 'subnet', 'gateway', 'dns'];\n"                                                                                                                    \
                   "            fields.forEach(field => {\n"                                                                                                                                             \
                   "                const input = document.forms[0][field];\n"                                                                                                                           \
                   "                if (input.value.trim() === '') {\n"                                                                                                                                  \
                   "                    input.placeholder = 'e.g., 192.168.1.1';\n"                                                                                                                      \
                   "                    document.getElementById(field + '-error').textContent = 'This field is required.';\n"                                                                            \
                   "                    isValid = false;\n"                                                                                                                                              \
                   "                } else if (!ipPattern.test(input.value)) {\n"                                                                                                                        \
                   "                    document.getElementById(field + '-error').textContent = 'Invalid IP address format.';\n"                                                                         \
                   "                    isValid = false;\n"                                                                                                                                              \
                   "                }\n"                                                                                                                                                                 \
                   "            });\n"                                                                                                                                                                   \
                   "            if (!isValid) {\n"                                                                                                                                                       \
                   "                event.preventDefault();\n"                                                                                                                                           \
                   "            }\n"                                                                                                                                                                     \
                   "        }\n"                                                                                                                                                                         \
                   "    </script>\n"                                                                                                                                                                     \
                   "</head>\n"                                                                                                                                                                           \
                   "<body>\n"                                                                                                                                                                            \
                   "    <h1>W55MH32 Configuration Page</h1>\n"                                                                                                                                           \
                   "    <form method=\"post\" action=\"config.cgi\" onsubmit=\"validateForm(event)\">\n"                                                                                                 \
                   "        <label>MAC Address: </label><input type=\"text\" value=\"%s\" readonly><br>\n"                                                                                               \
                   "        <label>IP Address: </label><input type=\"text\" name=\"ip\" placeholder=\"e.g., 192.168.1.1\" value=\"%s\"><div id=\"ip-error\" class=\"error\"></div><br>\n"                \
                   "        <label>Subnet Mask: </label><input type=\"text\" name=\"subnet\" placeholder=\"e.g., 255.255.255.0\" value=\"%s\"><div id=\"subnet-error\" class=\"error\"></div><br>\n"     \
                   "        <label>Default Gateway: </label><input type=\"text\" name=\"gateway\" placeholder=\"e.g., 192.168.1.1\" value=\"%s\"><div id=\"gateway-error\" class=\"error\"></div><br>\n" \
                   "        <label>DNS Server: </label><input type=\"text\" name=\"dns\" placeholder=\"e.g., 8.8.8.8\" value=\"%s\"><div id=\"dns-error\" class=\"error\"></div><br>\n"                  \
                   "        <input type=\"submit\" value=\"Submit\">\n"                                                                                                                                  \
                   "    </form>\n"                                                                                                                                                                       \
                   "</body>\n"                                                                                                                                                                           \
                   "</html>\n"

#define CONFIG_SUCCESS_PAGE                                                                               \
    "<!DOCTYPE html>\n"                                                                                   \
    "<html lang=\"en\">\n"                                                                                \
    "<head>\n"                                                                                            \
    "    <meta charset=\"UTF-8\">\n"                                                                      \
    "    <meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">\n"                    \
    "    <title>Configuration Modification Succeeded</title>\n"                                           \
    "    <style>\n"                                                                                       \
    "        body {\n"                                                                                    \
    "            font-family: Arial, sans-serif;\n"                                                       \
    "            text-align: center;\n"                                                                   \
    "            padding-top: 100px;\n"                                                                   \
    "            background-color: #f0f0f0; \n"                                                           \
    "        }\n"                                                                                         \
    "        h1 {\n"                                                                                      \
    "            color: green;\n"                                                                         \
    "            animation: fadeInOut 2s infinite;\n"                                                     \
    "        }\n"                                                                                         \
    "        #countdown {\n"                                                                              \
    "            font-size: 24px;\n"                                                                      \
    "            margin-top: 20px;\n"                                                                     \
    "            opacity: 0; \n"                                                                          \
    "            animation: fadeIn 1s forwards;\n"                                                        \
    "            animation-delay: 1s; \n"                                                                 \
    "        }\n"                                                                                         \
    "        /* Define the keyframes for fade-in */\n"                                                    \
    "        @keyframes fadeIn {\n"                                                                       \
    "            from {\n"                                                                                \
    "                opacity: 0;\n"                                                                       \
    "            }\n"                                                                                     \
    "            to {\n"                                                                                  \
    "                opacity: 1;\n"                                                                       \
    "            }\n"                                                                                     \
    "        }\n"                                                                                         \
    "    </style>\n"                                                                                      \
    "</head>\n"                                                                                           \
    "<body>\n"                                                                                            \
    "    <h1>Configuration Modification Succeeded!</h1>\n"                                                \
    "    <p id=\"countdown\">Will redirect in 10 seconds. Please wait...</p>\n"                           \
    "<script>\n"                                                                                          \
    "        let seconds = 10;\n"                                                                         \
    "        const countdownElement = document.getElementById('countdown');\n"                            \
    "        const countdownInterval = setInterval(() => {\n"                                             \
    "            seconds--;\n"                                                                            \
    "            countdownElement.textContent = `Will redirect in ${seconds} seconds. Please wait...`;\n" \
    "            if (seconds === 0) {\n"                                                                  \
    "                clearInterval(countdownInterval);\n"                                                 \
    "                window.location.href = 'http://%d.%d.%d.%d/';\n"                                     \
    "            }\n"                                                                                     \
    "        }, 1000);\n"                                                                                 \
    "</script>\n"                                                                                         \
    "</body>\n"                                                                                           \
    "</html>"

#endif
