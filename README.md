# Multi-Tetris
싱글 혹은 2명 이상의 플레이어 (최대 4명)이 접속하여 게임을 즐길 수 있는 온라인 테트리스의 서버와 클라이언트를 구현한 코드이다.

## 테트리스 서버
다음과 같은 문자열을 Argument로 입력 받으며 실행된다.

```console
./tetris_server port
```
* Port는 서버의 동작할 포트 번호이며, 생략 가능하다. 생략된 경우 기본 포트 번호 10001을 사용한다.

클라이언트가 서버에 접속할 경우, 최대 4명의 클라이언트 까지는 접속을 허가한다. 게임이 시작하면 이 후 접속하는 모든 클라이언트를 거부한다.

## 테트리스 클라이언트
다음과 같은 문자열을 Argument로 입력 받으며 실행된다. 

```console
./tetris_client Host IP address[:port][/ID]
```

* Host IP address는 dotted decimal notation(예를 들어, 1.2.3.4)을 의미한다. 
* Port는 접속할 서버의 포트 번호이며, 생략 가능하다. 생략된 경우, 기본 포트 번호 10001을 사용한다.  
* ID의 경우 생략이 가능하며 생략한 ID는 Anonymous로 간주한다.

서버와 접속이 성공하면, 테트리스 화면 하나가 뜨고 스페이스바를 통해 싱글 게임을 실행하거나 Enter를 통해 다른 참가자를 기다리는 Ready 상태에 있을 수 있다. 또 다른 참가자가 서버에 접속하거나 싱글 게임 중간에 ESC를 누르면 클라이언트에서 수행하던 게임은 일시 정지한다. 일시정지 상태에서 Enter를 통해 Ready 상태로 변경하거나 스페이스바로 일시정지 상태를 풀 수 있다.

### 싱글 플레이 화면 예시
![image](https://user-images.githubusercontent.com/62214506/78421380-089c8680-7692-11ea-854a-1c722650fc35.png)
![image](https://user-images.githubusercontent.com/62214506/78421382-0d613a80-7692-11ea-9b16-4cb81408faae.png)
![image](https://user-images.githubusercontent.com/62214506/78421383-0e926780-7692-11ea-8d7b-983efcbfbbf2.png)

모든 Client가 Ready상태가 되면 3초 Count가 출력이 되며 게임이 실행된다. 멀티 게임이 시작되면, 키를 통한 일시정지는 없다. 

### 4명의 멀티 플레이 예시
![image](https://user-images.githubusercontent.com/62214506/78421386-0f2afe00-7692-11ea-88d2-66047ccb7f93.png)

## 테트리스 조작방법
* W : Block 회전 
* A : Block 왼쪽 이동 
* D : Block 오른쪽 이동 
* S : Block 아래쪽 이동 
* E : Block 저장 및 불러오기
* 스페이스바 : Block 바로 내리기
