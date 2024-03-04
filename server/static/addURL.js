function addURL(url) {
    return fetch("/createURL", {
        method: "POST",
        body: JSON.stringify({
            url: url
        })
    });
}