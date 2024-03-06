function checkFormURL(event, url) {
    var customUrl = document.getElementById('customUrl').value;
    var longUrl = document.getElementById('longUrl').value;


    var resultElement = document.getElementById('result');
    resultElement.innerHTML = "Custom URL: qli.ink/" + customUrl + " will redirect to " + longUrl;
}

async function isUrlAvailable(url) {
    const response = await fetch("/isavail", {
        method: "POST",
        body: JSON.stringify({
            customUrl: url
        })
    });

    const data = await response.json();
    console.log(data);
    return data;
}