//functions define
/*  arg_1 id,
  * arg_2 uuid
  * arg_3 type
  * arg_4 wired: auto or not | wireless: encrypt or not 
  * arg_5 strength
  */
function js_insertItem(arg_1, arg_2, arg_3, arg_4, arg_5) {
    var isWireless = (arg_3==="802-11-wireless"?true : false);
    var item = '<div class="item" id="' + arg_2 +'">' +
            '<div><div class="icon">'+
            '<img src="qrc:/img/'+ ( isWireless ?"wireless":"wired") + '.png" alt="wired"></img>'+
            '</div><div class="id">'+ arg_1 +'</div><div class="status"></div>' + 
            (isWireless ? '<div class="strength"><div class="cover"></div></div>':'') + '</div>' + 
             '<div class="setting">' +
             '<div class="connect button" uuid=\"' + arg_2 + '\">'+
            '<div class="off"><span>关</span></div> <div class="vr"></div>'+ 
            '<div class="on"><span>开</span></div><div class="switch"></div></div>' + 
             '</div></div>'; 
    if(isWireless) {
        $(".wireless").find(".items").append($(item));
        $(item).find(".cover").width($(item).find('.Strength').width() - 
                                     $(item).find('.Strength').width() * arg_5 / 100);
    }
    else {
        $(".wired").find(".items").append($(item));
    }
}

function remove(uuid) {
    $("#" + uuid).remove();
}
/** init UI
  *  network is enabled or not
  * wireless is enabled or not
  */
function load() {
    if(Q_Network.networkUp) {
        $(".wired").show().parent().next().hide();
        if(!Q_Network.wirelessHardwareUp || !Q_Network.wirelessUp) {
            $(".wireless").hide();
        }
    }
    else {
        $("#connections").hide();
        $("#notify").show();
    }
}

function updateProperty(uuid, key, value) {
    //get refered uuid node
    $item = $("#" + uuid);
    if(key === 'Ssid') {
        $item.find(".id").text(value);
    }
    else if(key === 'Strength') {
        $item.find(".cover").width($item.find('.strength').width() - 
                                   $item.find(".strength").width() * str / 100);
    }
}

function button(target) {
    if(target.hasClass("enabled")) {
        //off
        target.removeClass("enabled").find(".switch").css('margin-left', '0');
        return 'off';
    }
    else {
        target.addClass("enabled").find(".switch").css('margin-left', '30px');
        return 'on';
    }
}

//actions
$(document).ready(function(){
    $(document).on("click",".item", function() {
       /* if($(this).find(".setting").is(':hidden'))
            $(this).find(".setting").show();
        else {
            $(this).find(".setting").hide();
        }*/
        $(this).find(".setting").show();
    })
    .on("mouseover", ".item", function() {
        $(this).css({background: "#BEBEBE"});
    })
    .on("mouseleave", ".item",function() {
        $(this).find(".setting").hide();
        $(this).css({background: "#9F9F9F"});
    })
    .on("click", ".connect", function() {
        if(button($(this)) === 'on') {
            Q_Network.tryConnect($(this).attr("uuid"));
        }
        else {
            Q_Network.tryConnect($(this).attr("uuid"));
        }
    });
    
    $("#flying_mode").on("click", function(){
        if(button($(this)) === 'off') {
            Q_Network.enableWireless(true);
        }
        else {
            Q_Network.enableWireless(false);
        }
    });
    
    $("#network-forbid").on("click", function() {
        if(button($(this)) === 'off') {
            Q_Network.enableNetwork(true);
        }
        else {
            Q_Network.enableNetwork(false);
        }
    });
    
    $("#popup").on("click", function() {
        $("#advanced").show();
    });
    
    $("#advanced").on("mouseleave", function() {
        $(this).hide();
    });
    
    $("#popdown").on("click", function(event) {
        $(this).parent().hide();
        event.stopPropagation();
    });
    
    $(".action").on("mouseover", function () {
        $(this).css('background-color', '#BEBEBE');    
    })
    .on("mouseleave", function () {
        $(this).css('background-color', '#8E8E8E');    
    });
    
    $("#connection-new").on("click", function() {
        Q_Operator.nmEditor();
    });
    
    //here connect javascript function to qt signals
    Q_Network.loadFinished.connect(load);
    Q_Operator.removeItem.connect(remove);
    Q_Network.accessPointProperty.connect(updateProperty);
});
