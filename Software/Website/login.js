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


//LOGIN
var btn = document.querySelector('button');

btn.onclick = function() {
    var inputUser = document.querySelector(".user-name").value;
    var inputPass = document.querySelector(".password").value;

    var status = document.querySelector('.status');

    firebase.database().ref('/DOAN1/login').once('value', function(snapshot) {
        var loginData = snapshot.val();
        console.log(loginData);
        
        if (loginData && loginData.username === inputUser && loginData.password === inputPass) {
            console.log("Login succes");
            status.textContent = "Login succes";
            window.location.href = "content.html";
        } else {
            console.log("Login failure");
            status.textContent = "Wrong username or password!";
        }
    });
}


