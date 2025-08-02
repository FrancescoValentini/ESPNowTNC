// Page elements
const outputBox = document.getElementById('console-textarea');
const commandInput = document.getElementById("txtbCommandInput");
const sendButton = document.getElementById('send-btn');
const modeSelect = document.getElementById('mode-select');

// Function to simulate sending to serial
function txToSerial(data) {
    // For now, just log it
    console.info("Data sent to serial:", data);
}

function fromHex(str1) {
    // Convert the input hexadecimal string to a regular string
    var hex = str1.toString();
    hex = hex.replaceAll(" ", ""); // remove spaces
    hex = hex.replaceAll("0x",""); // remove 0x...
    // Initialize an empty string to store the resulting ASCII characters
    var str = '';
    // Iterate through the hexadecimal string, processing two characters at a time
    for (var n = 0; n < hex.length; n += 2) {
        // Extract two characters from the hexadecimal string and convert them to their ASCII equivalent
        str += String.fromCharCode(parseInt(hex.substr(n, 2), 16));
    }
    // Return the resulting ASCII string
    return str;
}



// Handle sending logic
function handleSend() {
    const mode = modeSelect.value;
    const command = commandInput.value.trim();
    if (!command) return; // Do nothing if input is empty

    let dataToSend = command;

    if (mode === 'hex') {
        dataToSend = fromHex(command);
    }

    txToSerial(dataToSend);
    outputBox.value += '[' + geTime() + `] TX: ${command}\n`;

    commandInput.value = '';
}

function geTime(){
    return new Date().toTimeString().split(' ')[0];
}

// Handle Enter key press
commandInput.addEventListener('keydown', function(event) {
    if (event.key === 'Enter') {
        event.preventDefault();
        handleSend();
    }
});

// Handle click on SEND button
sendButton.addEventListener('click', handleSend);