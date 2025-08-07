# 💬 Encrypted Terminal-Based Chat Application

A lightweight, terminal-based client-server chat application written in **C++11** using **WinSock2** for socket programming and **multithreading** for concurrent client handling. The chat messages are encrypted using a simple Caesar cipher (for demo purposes), and the system can be easily extended to support modern encryption algorithms like AES.

---

## 🚀 Features

- 🔐 Caesar cipher encryption/decryption for message privacy  
- 🔄 Real-time broadcast messaging to multiple clients  
- 🧵 Multithreaded client handling with `std::thread` and `std::mutex`  
- 💻 Terminal-based interaction for both client and server  
- ✅ Clean and modular code for easy modification  

---

## 🛠️ Requirements

- Windows OS  
- g++ with `-std=c++11` support (e.g., MinGW)  
- WinSock2 library (`-lws2_32`)  

---

## 📦 Compilation

Open **Command Prompt** or terminal and navigate to the project directory:

```bash
g++ -std=c++11 chat_application.cpp -o chat -lws2_32 -static -pthread
```

> Make sure you have g++ and WinSock2 installed properly.

---

## 🧪 How to Run

### Start the Server

```bash
chat server
```

You'll see:

```
Server listening on port 8080...
```

### Connect a Client (in a new terminal window)

```bash
chat client 127.0.0.1
```

> Replace `127.0.0.1` with the actual IP address if on a network.

### Sample Client Interaction

```
Connected to server. Type 'exit' to quit.
> Hello everyone!
```

Encrypted messages are sent to the server and decrypted before broadcasting.

---

## 🔐 Encryption

The Caesar cipher shifts each alphabetical character by a fixed `KEY` (set to 5).  
This is for demo only – replace it with AES or other secure encryption algorithms for production use.

---

## 🧼 Clean Exit

Type `exit` in the client to disconnect gracefully.

```bash
> exit
Disconnected from server
```

---

## 📁 File Structure

```
chat_application.cpp  # Main server-client source code
README.md             # Project documentation
```


## 📽️ Demo Video

🎥 [Watch the Demo](https://drive.google.com/file/d/1bZsZHe3rnObXqX4f2Gav5vPrd-_6-z_I/view?usp=drivesdk)



---

## ✨ Future Enhancements

- Replace Caesar cipher with AES-256  
- Add authentication for clients  
- Cross-platform support (Linux/Mac using `select`/`poll`)  
- GUI version using Qt or WinAPI  

---




© 2025 Mahak Singla. All rights reserved.
