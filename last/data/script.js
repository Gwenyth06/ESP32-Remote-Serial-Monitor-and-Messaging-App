var gateway = `ws://${window.location.hostname}/ws`;
var websocket;

var displayer = document.getElementById("displayer");
var wsStatus = document.getElementById("ws-status");
var wsStatusDiv = document.getElementById("ws-status-div");

function onLoad(event) {
    initWebSocket();
}

function initWebSocket() {
    console.log('Trying to open a WebSocket connection...');

    websocket = new WebSocket(gateway);
    websocket.onopen = onOpen;
    websocket.onclose = onClose;
    websocket.onmessage = onMessage;
}

function onOpen(event) {
    console.log('Connection opened');

    wsStatusDiv.style.backgroundColor = "green";
    wsStatus.textContent = "ON";

    
    const messages = JSON.parse(localStorage.getItem("messages"));

    if (!messages) {
        localStorage.setItem("messages", JSON.stringify([]));
    } else {
        messages.forEach(message => {
            createMessageHTML(message);
        });
    }
}

function onClose(event) {
    console.log('Connection closed');
    wsStatusDiv.style.backgroundColor = "red";
    wsStatus.textContent = "OFF";
    setTimeout(initWebSocket, 2000);
}

function onMessage(event) { 
    var myMessage = JSON.parse(event.data);
    if (myMessage.text == undefined) {
        var serialMessage = {
            text: myMessage,
            date: new Date().toISOString()
        }
        myMessage = serialMessage;
    }

    var messages = JSON.parse(localStorage.getItem("messages"));
    messages.push(myMessage);
    localStorage.setItem("messages", JSON.stringify(messages));

    createMessageHTML(myMessage);

    displayer.scrollTop = displayer.scrollHeight;
}

function submitMessage() {

    var input = document.getElementById("input").value;

    var message = {
        text: input,
        date: new Date().toISOString()
    }

    var messages = JSON.parse(localStorage.getItem("messages"));

    if (message == null) {
        console.log("Message is null.")
    } else {
        messages.push(message);

        localStorage.setItem("messages", JSON.stringify(messages));

        websocket.send(JSON.stringify(message));
    }
}

function logoutButton() {
    var xhr = new XMLHttpRequest();
    xhr.open("GET", "/logout", true);
    xhr.send();
    setTimeout(function () { window.open("/logged-out", "_self"); }, 1000);
}

createMessageHTML = (message) => {
    const messageUl = document.createElement("ul");
    const messageLi = document.createElement("li");
    const messageContext = document.createElement("span");
    const messageDate = document.createElement("span");

    messageDate.setAttribute("class", "date");

    messageDate.textContent = message.date;
    messageContext.textContent = message.text;

    messageContext.appendChild(messageDate);
    messageLi.appendChild(messageContext);
    messageUl.appendChild(messageLi)
    displayer.appendChild(messageUl);
}

window.addEventListener('load', onLoad);