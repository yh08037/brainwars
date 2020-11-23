# brainwars


## Progress

### 2020.11.21
#### 다중 사용자 접속 가능한 서버 테스트
소켓 통신을 통한 단방향(클라이언트->서버) 문자열 메시지 송수신 구현
![simple_server](images/simple-multi-user-server.png)

### 2020.11.22
#### 양방향 송수신 구현
쓰레드를 통한 양방향(클라이언트<->서버) 문자열 메시지 송수신 구현

참고: [FD_SET, FD_ZERO등의 매크로 함수 정리](http://blog.naver.com/tipsware/220810795410)
![bidirectional_msg](images/basic-txrx-complete.png)

#### LED Matrix 이해 및 응용
LED matrix 이해 및 원하는 원하는 대로 출력
원하는 위치에 원하는 색을 표시할 수 있도록 구현함

### 2020.11.23
#### joystick 이해
joystick의 입력을 받을 수 있는 디바이스 드라이버 구현