function addURL(url) {
    return fetch("/createURL", {
        method: "POST",
        body: JSON.stringify({
            url: url
        })
    });
}

function isValidURL(url) {
    try {
        const url_ = new URL(url);
    }
    catch (_) {
        return false;
    }
    return true;
}

function isUrlAvailable(url) {
    return fetch("/isavail", {
        method: "POST",
        body: JSON.stringify({
            customUrl: url
        })
    })
        .then(response => response.json())
        .then(data => data[1]);


}

function submitURL(event) {
    event.preventDefault();
    const urlInput = document.getElementById("urlInput").value;
    if (!isValidURL(urlInput)) {
        document.getElementById("result").innerText = "Please enter a valid URL";
    }
    else {
        addURL(urlInput)
            .then(response => response.json())
            .then(data => {
                // Create an anchor element
                const anchor = document.createElement("a");

                // Set the href attribute to the returned URL
                anchor.href = anchor.innerText = "https://qli.ink" + data[1];

                // Make the anchor open in a new tab
                anchor.target = "_blank";

                const copyButton = document.createElement("button");
                copyButton.innerText = "Copy to Clipboard";
                copyButton.className = "copyButton";
                copyButton.addEventListener("click", function() {
                    // Copy URL to clipboard
                    navigator.clipboard.writeText(anchor.href)
                        .then(() => {
                            copyButton.innerText = "Copied!";
                        })
                        .catch(err => {
                            console.error('Failed to copy URL: ', err);
                            alert('Failed to copy URL');
                        });
                });

                // Append the anchor to the result element
                document.getElementById("result").innerHTML = '';
                document.getElementById("result").appendChild(anchor);
                document.getElementById("result").appendChild(copyButton);
            })
            .catch(error => {
                // Handle errors
                console.error("Error adding URL:", error);
                document.getElementById("result").innerText = "Error adding URL. Please try again.";
            });
    }
}