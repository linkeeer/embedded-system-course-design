/****************************************************************************
* ui.h extension file, inclued from the uic-generated form implementation.
**
** If you want to add, delete, or rename functions or slots, use
** Qt Designer to update this file, preserving your code.
**
** You should not define a constructor or destructor in this file.
** Instead, write your code in functions called init() and destroy().
** These will automatically be called by the form's constructor and
** destructor.
*****************************************************************************/
#define HOST_ADDR "192.168.1.140"
#define PORT 9415
#define STATE_OPEN 1000
#define SET_SPEED 1001
#define STATE_CHECK 1002 
#define STATE_CLOSE 1003

void Form::init(){
    host=HOST_ADDR;
    isCheck = true;
    socket = new QSocket(this);
    sthread=new SeaThread();
    buttonGroup1->setButton(1);
    lineEdit1->setText(host);
    connect(sthread,SIGNAL( connectToServer() ), this,
	    SLOT( connectToServer() ) );
    connect( socket, SIGNAL(connected()), this,
	     SLOT(socketConnected()) );
    connect( socket, SIGNAL(connectionClosed()), this,
	     SLOT(socketConnectionClosed()) );
    connect( socket, SIGNAL(readyRead()), this,
	     SLOT(socketReadyRead()) );
    connect( socket, SIGNAL(error(int)), this,
	     SLOT(socketError(int)) );
    sthread->start();
    sthread->pause.tryLock();
}


void Form::closeConnection()
{
    textLabel2->setText(QString::fromLocal8Bit("连接关闭"));
    printf("close socket\n");
    socket->clearPendingData();
    socket->close();
    if ( socket->state() == QSocket::Closing ) {
	// We have a delayed close.
	connect( socket, SIGNAL(delayedCloseFinished()),
		 SLOT(socketClosed()) );
    } else {
	// The socket is closed.
	socketClosed();
    }
}


void Form::sendToServer()
{
    // write to the server    
    if(socket->state()==QSocket::Connected){
	if(isCheck==true){
	    QString s=QString::number(STATE_CHECK)+"0000"+"\r\n";
	    socket->writeBlock( s.latin1(), s.length() );
	    socket->flush(); 
	    isCheck=false;
	}else{
	    int cmd=slider2->value();
	    QString s=QString::number(cmd).sprintf("%04d",cmd);
	    if(buttonGroup1->selectedId()==0||buttonGroup1->selectedId()==-1){
		s=QString::number(SET_SPEED)+s+"\r\n";
		//textLabel2->setText(s);
		socket->writeBlock( s.latin1(), s.length() );
		socket->flush();
	    }
	    else  if(buttonGroup1->selectedId()==1){
		s=QString::number(STATE_CLOSE)+s+"\r\n";
		//textLabel2->setText(s);
		socket->writeBlock( s.latin1(), s.length() );
		socket->flush();	
	    }else  textLabel2->setText(QString::fromLocal8Bit("按钮组错误"));
	}
    }else textLabel2->setText(QString::fromLocal8Bit("网络错误"));
}


void Form::socketReadyRead()
{
    // read from the server
    QString s=socket->readLine();
    QString sstate=s.mid(0,4);
    QString sspeed=s.mid(4);
    const char* ch=s.latin1();;
    if(ch!=NULL)printf("%s\n",ch);
    int nstate=sstate.toInt();
    int nspeed=sspeed.toInt();
    if(nstate==STATE_OPEN){
	textLabel2->setText(QString::fromLocal8Bit("开启"));
	 textLabel4->setText(sspeed);
	 slider1->setValue(nspeed);
    }else if(nstate==STATE_CLOSE){
	textLabel2->setText(QString::fromLocal8Bit("关闭"));
	 textLabel4->setText("0");
	 slider1->setValue(0);
    }else if(nstate==SET_SPEED){
	textLabel2->setText(QString::fromLocal8Bit("开启"));
	textLabel4->setText(sspeed);
	slider1->setValue(nspeed);
    }else if(nstate==STATE_CHECK){
	textLabel4->setText(sspeed);
	slider1->setValue(nspeed);
    }else textLabel2->setText(QString::fromLocal8Bit("设置转速错误"));
}


void Form::socketConnected()
{
    //    textLabel2->setText("connected");
}


void Form::socketConnectionClosed()
{
    //   textLabel2->setText("disconnected");
    Q_UINT16 port=PORT;
    socket->connectToHost( host, port );
}


void Form::socketClosed()
{
    //    textLabel2->setText("socketclosed");
}

void Form::socketError( int e )
{
    QString s=QString::number(e);
    s="error:"+s;
    textLabel2->setText(s);
}

void Form::closeDialog(){
    emit endConnection();
   exit(0);
}

void Form::connectToServer()
{
    if(socket->state()!=QSocket::Connected){
	textLabel2->setText(QString::fromLocal8Bit("正在连接..."));
	Q_UINT16 port=PORT;
	socket->connectToHost( host, port );
    }else if(socket->state()==QSocket::Connected){
	isCheck=true;
	emit sendCheck();
    }
}


void Form::setHost()
{
    host=lineEdit1->text();
}


void Form::setRun()
{    
   // textLabel2->setText(QString::number(sthread->pause.locked()));
    if(sthread->pause.locked()){
	sthread->pause.unlock();
    }
}


void Form::setWaitAndDisconnect()
{
   // textLabel2->setText(QString::number(sthread->pause.locked()));
    if(sthread->pause.locked()==false){
	sthread->pause.tryLock();
	if(socket->state()!=QSocket::Idle){
	    emit endConnection();
	}
    }
}


void Form::setspinBox1()
{
    spinBox1->setValue(slider2->value());
}
