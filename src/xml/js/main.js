//functions define
function js_insertItem(id, uuid, type, auto) {
    item = '<div class="item" uuid="' + uuid +'">' +
            '<div>' + 
            '<div class="icon"><img src="qrc:/img/wired.png" alt="wired"></img></div> ' + 
            '<div class="id">'+ id +'</div>' + 
            '<div class="status"></div>' + 
            '</div>' + 
            '<div class="setting">' +
            '<div><img src="qrc:/img/key.png" alt="KEY"></img></div>' +
            '<div><img src="qrc:/img/setting.png" alt="setting"></img></div>' +
            '<div class="connect" uuid="' + uuid +'"><img src="qrc:/img/setting.png" alt="setting"></img></div>' + 
            '</div></div>';
    $(".connection").append($(item));
}

function load() {
    if(Q_Network.networkUp) {
        //allow showing device list
        $("#device-list").show();
        $("#network").addClass("enabled");
        
        if(Q_Network.wirelessUp) {
            $("#wireless").addClass("enabled");
        }
        else {
            $("#wireless").removeClass("enabled");
        }
    }
    else {
        $("#network").removeClass("enabled");
    }
}

//actions
$(document).ready(function(){
    $(document).on("click",".item", function() {
        $(this).find(".setting").show();
    });
    
    $(document).on("mouseleave", ".item",function() {
        $(this).find(".setting").hide();
    });
    
    $(document).on("click", ".connect", function() {
        Q_Network.tryConnect($(this).attr("uuid"));
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
    
    $("#network").on("click", function(){
        alert("enable network");
    });
    $("#wireless").on("click", function(){
        alert("enabled wireless");
    });
    $("#new-connection").on("click", function(){
        alert("new-connection");
    });
    
    //here connect javascript function to qt signals
    Q_Network.loadFinished.connect(load);
});
