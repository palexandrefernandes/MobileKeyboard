# MobileKeyboard
## Send keyboard inputs from you smartphone to your Windows Machine
### Made as a learning opportunity, functionality may vary

This project was made with C/C++ TCP Sockets and React-Native as the front-end of it.
1. Server uses windows sockets (WS2) as its way of communicating with the client and as such it will only run on Windows machines.
2. The React-Native app should function correctly in both iOS and Android but it is only setup for the later, no garanties on the former (might need more setup)

Fetures that it doesn't support:
* Special characters
* OS Short Cuts
* Keys like Shift, Ctrl and many more

Use at own discretion, this was made in a single day and it might not get a single future update. Also there are no security mesures in place for the communication between the server and the client and, for this reason, you should only use this on trusted networks or you might get all your packages sniffed.
