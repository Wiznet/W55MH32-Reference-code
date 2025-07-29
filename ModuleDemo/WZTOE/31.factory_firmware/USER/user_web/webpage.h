#ifndef _WEBPAGE_H_
#define _WEBPAGE_H_

/*************************************************************************************
 * HTML Pages (web pages)
 *************************************************************************************/

#define HTML_PAGE                                                                                                                                                                                           \
    "<!DOCTYPE html>\n"                                                                                                                                                                                     \
    "<html lang=\"en\">\n"                                                                                                                                                                                  \
    "<head>\n"                                                                                                                                                                                              \
    "    <meta charset=\"UTF-8\">\n"                                                                                                                                                                        \
    "    <meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">\n"                                                                                                                      \
    "    <title>W55MH32 用户配置界面</title>\n"                                                                                                                                                       \
    "    <link rel=\"icon\" href=\"data:,\">\n"                                                                                                                                                             \
    "    <!-- CSS -->\n"                                                                                                                                                                                    \
    "    <style>\n"                                                                                                                                                                                         \
    "        body {\n"                                                                                                                                                                                      \
    "            font-family: Arial, sans-serif;\n"                                                                                                                                                         \
    "            text-align: center;\n"                                                                                                                                                                     \
    "            margin: 20px;\n"                                                                                                                                                                           \
    "        }\n"                                                                                                                                                                                           \
    "        .container {\n"                                                                                                                                                                                \
    "            width: 80%;\n"                                                                                                                                                                             \
    "            margin: auto;\n"                                                                                                                                                                           \
    "            border: 1px solid #ccc;\n"                                                                                                                                                                 \
    "            padding: 20px;\n"                                                                                                                                                                          \
    "            box-shadow: 0 0 10px rgba(0, 0, 0, 0.1);\n"                                                                                                                                                \
    "        }\n"                                                                                                                                                                                           \
    "        .section {\n"                                                                                                                                                                                  \
    "            margin: 20px 0;\n"                                                                                                                                                                         \
    "            padding: 10px;\n"                                                                                                                                                                          \
    "            border: 1px solid #ccc;\n"                                                                                                                                                                 \
    "            height: auto;\n"                                                                                                                                                                           \
    "            display: flex;\n"                                                                                                                                                                          \
    "            flex-direction: column;\n"                                                                                                                                                                 \
    "            justify-content: flex-start;\n"                                                                                                                                                            \
    "            align-items: center;\n"                                                                                                                                                                    \
    "        }\n"                                                                                                                                                                                           \
    "        button {\n"                                                                                                                                                                                    \
    "            background-color: #4CAF50;\n"                                                                                                                                                              \
    "            color: white;\n"                                                                                                                                                                           \
    "            border: none;\n"                                                                                                                                                                           \
    "            padding: 8px 16px;\n"                                                                                                                                                                      \
    "            font-size: 16px;\n"                                                                                                                                                                        \
    "            cursor: pointer;\n"                                                                                                                                                                        \
    "            border-radius: 5px;\n"                                                                                                                                                                     \
    "            margin: 5px;\n"                                                                                                                                                                            \
    "        }\n"                                                                                                                                                                                           \
    "        button:hover {\n"                                                                                                                                                                              \
    "            background-color: #45a049;\n"                                                                                                                                                              \
    "        }\n"                                                                                                                                                                                           \
    "        .textbox {\n"                                                                                                                                                                                  \
    "            width: 90%;\n"                                                                                                                                                                             \
    "            height: 200px;\n"                                                                                                                                                                          \
    "            margin: 10px 0;\n"                                                                                                                                                                         \
    "            display: block;\n"                                                                                                                                                                         \
    "            border: 1px solid #ccc;\n"                                                                                                                                                                 \
    "            padding: 10px;\n"                                                                                                                                                                          \
    "            resize: none;\n"                                                                                                                                                                           \
    "            box-sizing: border-box;\n"                                                                                                                                                                 \
    "            overflow-y: auto;\n"                                                                                                                                                                       \
    "        }\n"                                                                                                                                                                                           \
    "        .input {\n"                                                                                                                                                                                    \
    "            width: 90%;\n"                                                                                                                                                                             \
    "            padding: 10px;\n"                                                                                                                                                                          \
    "            margin: 10px 0;\n"                                                                                                                                                                         \
    "            border: 1px solid #ccc;\n"                                                                                                                                                                 \
    "            box-sizing: border-box;\n"                                                                                                                                                                 \
    "        }\n"                                                                                                                                                                                           \
    "        .row {\n"                                                                                                                                                                                      \
    "            display: flex;\n"                                                                                                                                                                          \
    "            justify-content: space-between;\n"                                                                                                                                                         \
    "            align-items: flex-start;\n"                                                                                                                                                                \
    "            gap: 10px;\n"                                                                                                                                                                              \
    "        }\n"                                                                                                                                                                                           \
    "        .column {\n"                                                                                                                                                                                   \
    "            width: 30%;\n"                                                                                                                                                                             \
    "            height: 350px;\n"                                                                                                                                                                          \
    "        }\n"                                                                                                                                                                                           \
    "        .info-section {\n"                                                                                                                                                                             \
    "            display: flex;\n"                                                                                                                                                                          \
    "            justify-content: space-between;\n"                                                                                                                                                         \
    "            align-items: center;\n"                                                                                                                                                                    \
    "            margin-bottom: 10px;\n"                                                                                                                                                                    \
    "        }\n"                                                                                                                                                                                           \
    "        .info {\n"                                                                                                                                                                                     \
    "            font-size: 16px;\n"                                                                                                                                                                        \
    "            margin: 0 5px;\n"                                                                                                                                                                          \
    "        }\n"                                                                                                                                                                                           \
    "        .button-container {\n"                                                                                                                                                                         \
    "            display: flex;\n"                                                                                                                                                                          \
    "            justify-content: space-evenly;\n"                                                                                                                                                          \
    "            align-items: center;\n"                                                                                                                                                                    \
    "            flex-wrap: wrap;\n"                                                                                                                                                                        \
    "            gap: 10px;\n"                                                                                                                                                                              \
    "        }\n"                                                                                                                                                                                           \
    "    </style>\n"                                                                                                                                                                                        \
    "</head>\n"                                                                                                                                                                                             \
    "<!-- body -->\n"                                                                                                                                                                                       \
    "<body onload='page_load()'>\n"                                                                                                                                                                         \
    "    <div class=\"container\">\n"                                                                                                                                                                       \
    "        <h1>W55MH32 用户配置界面</h1>\n"                                                                                                                                                         \
    "        <p id=\"current-time\"></p>\n"                                                                                                                                                                 \
    "        <div class=\"section\">\n"                                                                                                                                                                     \
    "            <div class=\"info-section\">\n"                                                                                                                                                            \
    "                <p class=\"info\">当前温度：<label id=\"temperature\">NULL</label>°C</p>\n"                                                                                                      \
    "                <p class=\"info\">当前湿度：<label id=\"humidity\">NULL</label>%RH</p>\n"                                                                                                         \
    "                <p class=\"info\">按键次数统计：<label id=\"button_cnt\">0</label>次</p>\n"                                                                                                    \
    "            </div>\n"                                                                                                                                                                                  \
    "            <div class=\"button-container\">\n"                                                                                                                                                        \
    "                <button val='1' name='led' onclick=\"set_io_state(this)\">LED开</button>\n"                                                                                                           \
    "                <button val='0' name='led' onclick=\"set_io_state(this)\">LED关</button>\n"                                                                                                           \
    "                <button val='1' name='beep' onclick=\"set_io_state(this)\">蜂鸣器开</button>\n"                                                                                                    \
    "                <button val='0' name='beep' onclick=\"set_io_state(this)\">蜂鸣器关</button>\n"                                                                                                    \
    "            </div>\n"                                                                                                                                                                                  \
    "        </div>\n"                                                                                                                                                                                      \
    "        <div class=\"row\">\n"                                                                                                                                                                         \
    "            <div class=\"column section\">\n"                                                                                                                                                          \
    "                <p>SD卡</p>\n"                                                                                                                                                                        \
    "                <textarea class=\"textbox\" readonly id=\"sd-textbox\" placeholder=\"SD卡内容显示\"></textarea>\n"                                                                                \
    "                <button onclick=\"readContent()\">读取内容</button>\n"                                                                                                                             \
    "                <button onclick=\"writeContent()\">写入内容</button>\n"                                                                                                                            \
    "                <input type=\"file\" id=\"fileInput\" onchange=\"choose_file()\" accept=\".txt\" style=\"display: none;\" />\n"                                                                        \
    "            </div>\n"                                                                                                                                                                                  \
    "            <div class=\"column section\">\n"                                                                                                                                                          \
    "                <p>网络配置</p>\n"                                                                                                                                                                 \
    "                <input class=\"input\" type=\"text\" id=\"ip-address\" placeholder=\"IP地址\">\n"                                                                                                    \
    "                <input class=\"input\" type=\"text\" id=\"subnet-mask\" placeholder=\"子网掩码\">\n"                                                                                               \
    "                <input class=\"input\" type=\"text\" id=\"default-gateway\" placeholder=\"默认网关\">\n"                                                                                           \
    "                <input class=\"input\" type=\"text\" id=\"dns\" placeholder=\"DNS\">\n"                                                                                                                \
    "                <button onclick=\"saveConfiguration()\">保存并配置</button>\n"                                                                                                                    \
    "            </div>\n"                                                                                                                                                                                  \
    "            <div class=\"column section\">\n"                                                                                                                                                          \
    "                <p>USB串口</p>\n"                                                                                                                                                                    \
    "                <textarea class=\"textbox\" readonly id=\"usb-textbox\" placeholder=\"USB串口显示\"></textarea>\n"                                                                                 \
    "                <input class=\"input\" type=\"text\" id=\"usb-input\" placeholder=\"输入消息\">\n"                                                                                                 \
    "                <button onclick=\"sendMessage()\">发送消息</button>\n"                                                                                                                             \
    "            </div>\n"                                                                                                                                                                                  \
    "        </div>\n"                                                                                                                                                                                      \
    "    </div>\n"                                                                                                                                                                                          \
    "    <script>\n"                                                                                                                                                                                        \
    "       <!-- AJAX -->\n"                                                                                                                                                                                \
    "       function AJAX(a,e){"                                                                                                                                                                            \
    "       	var c=d();"                                                                                                                                                                                    \
    "       	c.onreadystatechange=b;"                                                                                                                                                                       \
    "       	function d(){"                                                                                                                                                                                 \
    "       		if(window.XMLHttpRequest){"                                                                                                                                                                   \
    "       			return new XMLHttpRequest()"                                                                                                                                                                 \
    "       		}"                                                                                                                                                                                            \
    "       		else{"                                                                                                                                                                                        \
    "       			if(window.ActiveXObject){"                                                                                                                                                                   \
    "       				return new ActiveXObject(\"Microsoft.XMLHTTP\")"                                                                                                                                            \
    "       			}	"                                                                                                                                                                                          \
    "       		}"                                                                                                                                                                                            \
    "       	}"                                                                                                                                                                                             \
    "       	function b(){"                                                                                                                                                                                 \
    "       		if(c.readyState==4){"                                                                                                                                                                         \
    "       			if(c.status==200){"                                                                                                                                                                          \
    "       				if(e){"                                                                                                                                                                                     \
    "       					e(c.responseText)"                                                                                                                                                                         \
    "       				}"                                                                                                                                                                                          \
    "       			}"                                                                                                                                                                                           \
    "       		}"                                                                                                                                                                                            \
    "       	}"                                                                                                                                                                                             \
    "       	this.doGet=function(){"                                                                                                                                                                        \
    "       		c.open(\"GET\",a,true);"                                                                                                                                                                      \
    "       		c.send(null)"                                                                                                                                                                                 \
    "       	};"                                                                                                                                                                                            \
    "       	this.doPost=function(f){"                                                                                                                                                                      \
    "       		c.open(\"POST\",a,true);"                                                                                                                                                                     \
    "       		c.setRequestHeader(\"Content-Type\",\"application/x-www-form-urlencoded\");"                                                                                                                  \
    "       		c.setRequestHeader(\"ISAJAX\",\"yes\");"                                                                                                                                                      \
    "       		c.send(f)"                                                                                                                                                                                    \
    "       	}"                                                                                                                                                                                             \
    "       }"                                                                                                                                                                                              \
    "       function $(a){"                                                                                                                                                                                 \
    "       	return document.getElementById(a)"                                                                                                                                                             \
    "       }"                                                                                                                                                                                              \
    "       function $$(a){"                                                                                                                                                                                \
    "       	return document.getElementsByName(a)"                                                                                                                                                          \
    "       }"                                                                                                                                                                                              \
    "       function $$_ie(a,c){if(!a){"                                                                                                                                                                    \
    "       	a=\"*\""                                                                                                                                                                                       \
    "       }"                                                                                                                                                                                              \
    "       var b=document.getElementsByTagName(a);"                                                                                                                                                        \
    "       var e=[];"                                                                                                                                                                                      \
    "       for(var d=0;d<b.length;d++){"                                                                                                                                                                   \
    "       	att=b[d].getAttribute(\"name\");"                                                                                                                                                              \
    "       		if(att==c){"                                                                                                                                                                                  \
    "       			e.push(b[d])"                                                                                                                                                                                \
    "       		}"                                                                                                                                                                                            \
    "       	}"                                                                                                                                                                                             \
    "       	return e"                                                                                                                                                                                      \
    "       }"                                                                                                                                                                                              \
    "       <!-- led_beep_js -->\n"                                                                                                                                                                         \
    "       function set_io_state(o){\n"                                                                                                                                                                    \
    "       	var name=o.attributes['name'].value;\n"                                                                                                                                                        \
    "       	var val=o.attributes['val'].value;\n"                                                                                                                                                          \
    "       	dout=new AJAX('set_io_state.cgi', function(t){try{eval(t);}catch(e){alert(e);}});\n"                                                                                                           \
    "       	dout.doPost('name='+name+'&val='+val);\n"                                                                                                                                                      \
    "       }\n"                                                                                                                                                                                            \
    "       <!-- netinfo_js -->\n"                                                                                                                                                                          \
    "       function get_netinfo(){\n"                                                                                                                                                                      \
    "       	var oUpdate;\n"                                                                                                                                                                                \
    "       	setTimeout(function(){\n"                                                                                                                                                                      \
    "       	oUpdate=new AJAX('get_netinfo.cgi',function(t){\n"                                                                                                                                             \
    "       		try{eval(t);}catch(e){alert(e);}\n"                                                                                                                                                           \
    "       	});\n"                                                                                                                                                                                         \
    "       	oUpdate.doGet();},500);\n"                                                                                                                                                                     \
    "       }\n"                                                                                                                                                                                            \
    "       function set_netinfo(){\n"                                                                                                                                                                      \
    "       	const ip = document.getElementById('ip-address').value;\n"                                                                                                                                     \
    "       	const subnet = document.getElementById('subnet-mask').value;\n"                                                                                                                                \
    "       	const gateway = document.getElementById('default-gateway').value;\n"                                                                                                                           \
    "       	const dns = document.getElementById('dns').value;\n"                                                                                                                                           \
    "       	update_netinfo=new AJAX('set_netinfo.cgi',function(t){try{eval(t);}catch(e){alert(e);}});\n"                                                                                                   \
    "       	update_netinfo.doPost('ip='+ip+'&sn='+subnet+'&gw='+gateway+'&dns='+dns);\n"                                                                                                                   \
    "       }\n"                                                                                                                                                                                            \
    "       function get_netinfo_callback(o){\n"                                                                                                                                                            \
    "       	$('ip-address').value=o.ip;\n"                                                                                                                                                                 \
    "       	$('default-gateway').value=o.gw;\n"                                                                                                                                                            \
    "       	$('subnet-mask').value=o.sn;\n"                                                                                                                                                                \
    "       	$('dns').value=o.dns;\n"                                                                                                                                                                       \
    "       }\n"                                                                                                                                                                                            \
    "       <!-- update_page_js -->\n"                                                                                                                                                                      \
    "       function update_page(){\n"                                                                                                                                                                      \
    "       	setInterval(function () {\n"                                                                                                                                                                   \
    "       		update_page = new AJAX('get_update_page.cgi', function (t) { try { eval(t); } catch (e) { alert(e); } });\n"                                                                                  \
    "       		update_page.doGet();\n"                                                                                                                                                                       \
    "       	}, 1000);\n"                                                                                                                                                                                   \
    "       }\n"                                                                                                                                                                                            \
    "       function update_page_callback(o){\n"                                                                                                                                                            \
    "           $('temperature').innerHTML=o.temperature;\n"                                                                                                                                                \
    "       	$('humidity').innerHTML=o.humidity;\n"                                                                                                                                                         \
    "       	$('button_cnt').innerHTML=o.button_cnt;\n"                                                                                                                                                     \
    "           if(o.usbdata){\n"                                                                                                                                                                           \
    "               $('usb-textbox').value=o.usbdata+$('usb-textbox').value;\n"                                                                                                                             \
    "           }\n"                                                                                                                                                                                        \
    "       }\n"                                                                                                                                                                                            \
    "       <!-- web_usb_js -->\n"                                                                                                                                                                          \
    "       function web_send_usb_data(){\n"                                                                                                                                                                \
    "       	dout=new AJAX('web_send_usb_data.cgi', function(t){try{eval(t);}catch(e){alert(e);}});\n"                                                                                                      \
    "       	dout.doPost('data='+$('usb-input').value);\n"                                                                                                                                                  \
    "       }\n"                                                                                                                                                                                            \
    "       <!-- sd_js -->\n"                                                                                                                                                                               \
    "	    function get_sd_file_name(){\n"                                                                                                                                                                   \
    " 	    		$('sd-textbox').value=\'\';\n"                                                                                                                                                                 \
    "	    		dout=new AJAX('get_sd.cgi', function(t){try{eval(t);}catch(e){alert(e);}});\n"                                                                                                                  \
    "	    		dout.doGet();\n"                                                                                                                                                                                \
    "	    	}\n"                                                                                                                                                                                             \
    "	    function choose_file(){ \n"                                                                                                                                                                       \
    " 	    	file = $('fileInput').files[0];\n"                                                                                                                                                              \
    "	    	dout=new AJAX('put_sd.cgi', function(t){try{eval(t);}catch(e){alert(e);}});\n"                                                                                                                   \
    " 	    	if(file){\n"                                                                                                                                                                                    \
    " 	    		const reader = new FileReader();\n"                                                                                                                                                            \
    " 	    		reader.onload = function(e){\n"                                                                                                                                                                \
    "  	    		let content = e.target.result;\n"                                                                                                                                                             \
    "               if(content.length<100){\n"                                                                                                                                                              \
    "  	    		    dout.doPost(\"filename=\"+file.name+\"&filelen=\"+content.length+\"&filedata=\"+content);\n"                                                                                              \
    "               }\n"                                                                                                                                                                                    \
    "               else{\n"                                                                                                                                                                                \
    "                   alert('文件过大!');\n"                                                                                                                                                          \
    "               }\n"                                                                                                                                                                                    \
    "  	    		console.log(\"filename=\"+file.name+\"&filelen='\"+content.length+\"'&filedata=\"+content);\n"                                                                                                \
    "  	    		console.log('filedata:'+content);\n"                                                                                                                                                          \
    "  	    		console.log('filename:'+file.name);\n"                                                                                                                                                        \
    "  	    		console.log('filelen:'+content.length);\n"                                                                                                                                                    \
    "  	    	};\n"                                                                                                                                                                                          \
    "  	    	reader.readAsText(file);\n"                                                                                                                                                                    \
    "	    	}\n"                                                                                                                                                                                             \
    "	    	else{\n"                                                                                                                                                                                         \
    "	    		alert('请选择文件');\n"                                                                                                                                                                    \
    "	    	}\n"                                                                                                                                                                                             \
    "	    }\n"                                                                                                                                                                                              \
    "       <!-- update_time_js -->\n"                                                                                                                                                                      \
    "       function updateTime() {\n"                                                                                                                                                                      \
    "           const now = new Date();\n"                                                                                                                                                                  \
    "           const formattedTime = now.toLocaleString();\n"                                                                                                                                              \
    "           document.getElementById('current-time').innerText = `当前时间: ${formattedTime}`;\n"                                                                                                    \
    "       }\n"                                                                                                                                                                                            \
    "       setInterval(updateTime, 1000);\n"                                                                                                                                                               \
    "       updateTime();\n"                                                                                                                                                                                \
    "       function writeContent() {\n"                                                                                                                                                                    \
    "           document.getElementById('fileInput').value=\"\";\n"                                                                                                                                         \
    "           document.getElementById('fileInput').click();\n"                                                                                                                                            \
    "       }\n"                                                                                                                                                                                            \
    "       function readContent() {\n"                                                                                                                                                                     \
    "           get_sd_file_name();\n"                                                                                                                                                                      \
    "       }\n"                                                                                                                                                                                            \
    "       function isValidIPv4(ip) {\n"                                                                                                                                                                   \
    "           const regex = /^(25[0-5]|2[0-4][0-9]|[0-1]?[0-9][0-9]?)\\.(25[0-5]|2[0-4][0-9]|[0-1]?[0-9][0-9]?)\\.(25[0-5]|2[0-4][0-9]|[0-1]?[0-9][0-9]?)\\.(25[0-5]|2[0-4][0-9]|[0-1]?[0-9][0-9]?)$/;\n" \
    "           return regex.test(ip);\n"                                                                                                                                                                   \
    "       }\n"                                                                                                                                                                                            \
    "       function saveConfiguration() {\n"                                                                                                                                                               \
    "           const ip = document.getElementById('ip-address').value;\n"                                                                                                                                  \
    "           const subnet = document.getElementById('subnet-mask').value;\n"                                                                                                                             \
    "           const gateway = document.getElementById('default-gateway').value;\n"                                                                                                                        \
    "           const dns = document.getElementById('dns').value;\n"                                                                                                                                        \
    "           if (!isValidIPv4(ip)) {\n"                                                                                                                                                                  \
    "               alert('IP地址格式不正确！');\n"                                                                                                                                                 \
    "               return;\n"                                                                                                                                                                              \
    "           }\n"                                                                                                                                                                                        \
    "           if (!isValidIPv4(subnet)) {\n"                                                                                                                                                              \
    "               alert('子网掩码格式不正确！');\n"                                                                                                                                             \
    "               return;\n"                                                                                                                                                                              \
    "           }\n"                                                                                                                                                                                        \
    "           if (!isValidIPv4(gateway)) {\n"                                                                                                                                                             \
    "               alert('默认网关格式不正确！');\n"                                                                                                                                             \
    "               return;\n"                                                                                                                                                                              \
    "           }\n"                                                                                                                                                                                        \
    "           if (!isValidIPv4(dns)) {\n"                                                                                                                                                                 \
    "               alert('DNS地址格式不正确！');\n"                                                                                                                                                \
    "               return;\n"                                                                                                                                                                              \
    "           }\n"                                                                                                                                                                                        \
    "			set_netinfo();\n"                                                                                                                                                                                   \
    "       }\n"                                                                                                                                                                                            \
    "       function sendMessage() {\n"                                                                                                                                                                     \
    "           const input = document.getElementById('usb-input');\n"                                                                                                                                      \
    "           if (input.value.trim() !== \"\") {\n"                                                                                                                                                       \
    "               web_send_usb_data();\n"                                                                                                                                                                 \
    "           } else {\n"                                                                                                                                                                                 \
    "               alert('输入框不能为空！');\n"                                                                                                                                                   \
    "           }\n"                                                                                                                                                                                        \
    "       }\n"                                                                                                                                                                                            \
    "       function page_load() {\n"                                                                                                                                                                       \
    "           get_netinfo();\n"                                                                                                                                                                           \
    "			update_page();\n"                                                                                                                                                                                   \
    "       }\n"                                                                                                                                                                                            \
    "    </script>\n"                                                                                                                                                                                       \
    "</body>\n"                                                                                                                                                                                             \
    "</html>"

#endif
