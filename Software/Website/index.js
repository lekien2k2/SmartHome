//Kết nối firebase
const firebaseConfig = {
    apiKey: "AIzaSyAPGg0zclO9mVOUy1qQk9GwFilenQbwIoc",
    authDomain: "doan1-efece.firebaseapp.com",
    projectId: "doan1-efece",
    storageBucket: "doan1-efece.appspot.com",
    messagingSenderId: "672761800159",
    appId: "1:672761800159:web:d2476a81672d3f1e64906a",
    measurementId: "G-NVKJ6JNTD8"
  };

firebase.initializeApp(firebaseConfig);

// const firebaseConfig = {
//     apiKey: "AIzaSyDvaEkAsnoY72-vST9UwpX3bpIxSPk6_Lw",
//     authDomain: "damh1-c89a1.firebaseapp.com",
//     projectId: "damh1-c89a1",
//     storageBucket: "damh1-c89a1.appspot.com",
//     messagingSenderId: "434537061099",
//     appId: "1:434537061099:web:ed63bfb0516d2123ef3cae",
//     measurementId: "G-1LYEE64P58"
//   };

//   // Initialize Firebase
//   const app = initializeApp(firebaseConfig);





//CONTENT

$("#id-temp-round-slider").roundSlider({
    handleShape: "round",
    radius:130,
    value: 45,
    handleSize: "20,10",
    circleShape: "half-top",
    sliderType: "min-range",
    lineCap: "round",
    mouseScrollAction: true,
    width: 5,
    lineCap: "round",
    readOnly:true,
    // format giá lại hiển thị
    tooltipFormat: function(value) {
        return '<span style="font-size: 30px;">Nhiệt độ: ' + value.value + '°C</span>';
    },
    
});

$("#id-humid-round-slider").roundSlider({
    handleShape: "round",
    radius:130,
    value: 45,
    handleSize: "20,10",
    circleShape: "half-top",
    sliderType: "min-range",
    lineCap: "round",
    mouseScrollAction: true,
    width: 5,
    lineCap: "round",
    readOnly:true,
    // format giá lại hiển thị
    tooltipFormat: function(value) {
        return '<span style="font-size: 30px;">Độ ẩm: ' + value.value + '%</span>';
    },
});




// LED

$("#id-led-slider").on("change", function() {
    var sliderValue = $(this).val(); // Lấy giá trị của RangeSlider
    var elements = document.querySelector('.img-led'); 
    
    elements.style.opacity = sliderValue / 100;
});


//RÈM
//   ---------------------------------------------
// Lấy tham chiếu đến phần tử HTML chứa biểu tượng micro
const microIcon = document.querySelector('ion-icon[name="mic-off-outline"]');

// Thêm trình xử lý sự kiện cho sự kiện click
microIcon.addEventListener('click', () => {
    // Đổi tên biểu tượng thành mic, để hiển thị biểu tượng micro đang bật
    microIcon.setAttribute('name', 'mic-outline');
    document.querySelector('#btnTalk').style.backgroundColor = 'red';
  // Tự động tắt micro sau 5 giây
    setTimeout(() => {
    // Đổi tên biểu tượng thành mic-off, để hiển thị biểu tượng micro đang tắt
        microIcon.setAttribute('name', 'mic-off-outline');
        document.querySelector('#btnTalk').style.backgroundColor = 'green';
  }, 5000);
});


var message = document.querySelector('.message');
var reTalk = document.querySelector('#id-retalk');

var SpeechRecognition = SpeechRecognition || webkitSpeechRecognition;
var SpeechGrammarList = SpeechGrammarList || webkitSpeechGrammarList
var grammar = '#JSGF V1.0';
var recognition = new SpeechRecognition();
var speechRecognitionList = new SpeechGrammarList();
speechRecognitionList.addFromString(grammar, 1);
recognition.grammars = speechRecognitionList;
recognition.lang = 'vi-VN';
recognition.interimResults = false

var status = "OFF";

recognition.onresult = function(event) {
    var lastResult = event.results.length - 1;
    var content = event.results[lastResult][0].transcript;
    
    content = content.toLowerCase();
    // Phát hiện bật/tắt để chuyển thành ON/OFF
    if (content.includes("đóng") || content.includes("close") ){
        content = "OFF";
        reTalk.textContent = "";
    }
    else if (content.includes("mở") || content.includes("open")){
        content = "ON";
        reTalk.textContent = "";
    }
    else {
        reTalk.textContent = "Vui lòng nói lại!";
    }
    
    // message.textContent = content;
    myFunction(content);
};
recognition.onspeechend = function() {
    recognition.stop();
};
recognition.onerror = function(event) {
    reTalk.textContent = 'Vui lòng nói lại!';
};

document.querySelector('#btnTalk').addEventListener('click', function(){
    recognition.start();
}); 

// -----------------------------------------------------------------------

// const btn = document.querySelector('#id-switchtoggle');

function myFunction(curtainStatus){    
    if(curtainStatus === "ON"){
        // btn.checked = true;
        // cập nhật khi sử dụng giọng nói
        firebase.database().ref("/DOAN1").update({
            "curtain": "ON"
        })
    }
    if(curtainStatus  === "OFF"){
        // btn.checked = false;
        firebase.database().ref("/DOAN1").update({
            "curtain": "OFF"
        })
    } 
}

firebase.database().ref('/DOAN1/curtain').on('value', function(snapshot) {
    // Lấy giá trị từ Firebase Realtime Database
    var curtain = snapshot.val();
    // Cập nhật giá trị lên roundSlider
   if (curtain === "ON") {
        message.textContent = 'Đã mở';
   }
   else {
        message.innerText = 'Đã đóng';
   }
});




//NHIỆT ĐỘ, ĐỘ ẨM
// Lắng nghe sự kiện thay đổi giá trị trên Firebase Realtime Database
firebase.database().ref('/DOAN1/temperature').on('value', function(snapshot) {
    // Lấy giá trị từ Firebase Realtime Database
    var temperature = snapshot.val();
  
    // Cập nhật giá trị lên roundSlider
    $("#id-temp-round-slider").roundSlider("setValue", temperature);
});


firebase.database().ref('/DOAN1/humidity').on('value', function(snapshot) {
    // Lấy giá trị từ Firebase Realtime Database
    var humidity = snapshot.val();
  
    // Cập nhật giá trị lên roundSlider
    $("#id-humid-round-slider").roundSlider("setValue", humidity);
});


    

//KHÓA
var statusLock = document.querySelector('#id-status-lock');
var warningLock = document.querySelector('#id-warning-lock');
var warning = document.querySelector('#id-warning');
var toogleLock = document.querySelector('.lock');
var warningSoon = document.querySelector('.warning-soon');
//Lưu ID interval
var intervalId;
firebase.database().ref('/DOAN1/countLocker').on('value', function(snapshot) {
    // Lấy giá trị từ Firebase Realtime Database
    var countLocker = snapshot.val();
 
    if (countLocker == 0){
        
        warningSoon.style.backgroundColor = 'green';
        warningLock.innerText = 'An toàn';
        warningLock.style.color = 'green';
        warning.style.color = 'black';
        toogleLock.style.background = '#aeedf9';

        //trạng thái
        statusLock.innerText = 'Đã khóa';
        statusLock.style.color = 'red';
        
        
        if (intervalId) {
            clearInterval(intervalId);
        }
        if(toogleLock.style.visibility == 'hidden') {
            toogleLock.style.visibility = 'visible'
        }   
        if(warningSoon.style.visibility == 'hidden') {
            warningSoon.style.visibility = 'visible';
        }      

    }
    else if (countLocker == 1){
        warningSoon.style.backgroundColor = 'green';
        warningLock.innerText = 'An toàn';
        warningLock.style.color = 'green';
        warning.style.color = 'black';
        toogleLock.style.background = '#aeedf9';

        //trạng thái
        statusLock.innerText = 'Đã mở khóa';
        statusLock.style.color = 'red';
        
        
        if (intervalId) {
            clearInterval(intervalId);
        }
        if(toogleLock.style.visibility == 'hidden') {
            toogleLock.style.visibility = 'visible'
        }   
        if(warningSoon.style.visibility == 'hidden') {
            warningSoon.style.visibility = 'visible';
        } 
    }
    else if(countLocker == 2) {
        alert(`Đã mở khóa sai!`);
        warningSoon.style.backgroundColor = 'red';


        warning.style.color = 'red';
        warningLock.style.color = 'red';
        warningLock.innerText = `Đã mở khóa sai`;

        toogleLock.style.backgroundColor = 'orange';
        
        
        intervalId = setInterval(function() {
            toogleLock.style.visibility = (toogleLock.style.visibility == 'hidden') ? 'visible' : 'hidden';
            warningSoon.style.visibility = (warningSoon.style.visibility == 'hidden') ? 'visible' : 'hidden';
        }, 500);  
  
    }
    
});

//LED
$("#id-led-slider").on("input", function() {
    // Lấy giá trị trượt từ roundSlider
    var led = $(this).val();
    

    // Cập nhật giá trị lên Firebase Realtime Database
    firebase.database().ref('/DOAN1/led').set(led);
  
});

firebase.database().ref('/DOAN1/led').on('value', function(snapshot) {
    var updateLed = snapshot.val();
    console.log(updateLed);
    document.querySelector('#id-led-slider').value = updateLed;
});
