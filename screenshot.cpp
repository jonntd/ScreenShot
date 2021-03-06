#include "screenshot.h"
#include "ui_screenshot.h"
#include <QMessageBox>

ScreenShot::ScreenShot()
{
    ui->setupUi(this);
    tray = new QSystemTrayIcon;
    icon.addFile(":/new/prefix1/C:/Users/Fuerm/Desktop/1468956197_Screenshot.ico");
    tray->setIcon(icon);
    tray->show();
    creatActions();
    creatMenu();
    connect(tray,SIGNAL(activated(QSystemTrayIcon::ActivationReason)),
            this,SLOT(iconActivied(QSystemTrayIcon::ActivationReason)));
}

ScreenShot::~ScreenShot()
{
    delete ui;
}

void ScreenShot::pSize() //获取截图位置坐标
{
    pw = abs(end.x()-origin.x());
    ph = abs(end.y()-origin.y());
    px = origin.x()<end.x()?origin.x():end.x();
    py = origin.y()<end.y()?origin.y():end.y();
}

void ScreenShot::setColorLabel(int x,int y)//获取取色标签位置
{
    colorLabel->setText(tr("   R %1        \n   G %2        \n   B %3        \n   # %4 %5 %6   ")
          .arg(color.red()).arg(color.green()).arg(color.blue())
          .arg(QString::number(color.red(),16)).arg(QString::number(color.green(),16)).arg(QString::number(color.blue(),16)));
    font = new QFont();
    font->setPointSize(15);
    colorLabel->setFont(*font);
    colorLabel->show();
    QRect rect(colorLabel->contentsRect());
    if((width-x-20<=rect.width())&&(height-y>rect.height()))
            colorLabel->move(x-rect.width()-20,y);
    else if((width-x-20<=rect.width())&&(height-y<=rect.height()))
            colorLabel->move(x-rect.width()-20,y-rect.height());
    else if((width-x-20>rect.width())&&(height-y>rect.height()))
            colorLabel->move(x+20,y);
    else if((width-x-20>rect.width())&&(height-y<=rect.height()))
            colorLabel->move(x+20,y-rect.height());
}

void ScreenShot::setRubber() //设置截图框
{
    rubber = new  QRubberBand(QRubberBand::Rectangle,this);
    rubber->show();

    rubber->setGeometry(origin.x(),origin.y(),0,0);
}

void ScreenShot::keyPressEvent(QKeyEvent *e)
{
    switch(choice)
    {
    case 0:
        if(e->key()==Qt::Key_Escape)
        {
            this->hide();
        }
        break;
    case 1:
        if(e->key()==Qt::Key_Escape)
        {
            colorLabel->close();
            this->hide();
        }
    }
}

void ScreenShot::mousePressEvent(QMouseEvent *e) //鼠标按下
{
    origin = e->pos();
    switch(choice){
    case 0:
        if(e->button() == Qt::LeftButton)
        {
            if(shot==true)
            {
                setRubber();
                shot = false;
            }
            else
            {
                rubber->close();
                label->close();
                done->close();
                setRubber();
            }
            quit = false;
        }
        break;
    case 1:
        if(e->button() == Qt::LeftButton)
        {
            QString str = tr("RGB(%1,%2,%3)  \n# %4 %5 %6   ")
                    .arg(color.red()).arg(color.green()).arg(color.blue())
                    .arg(QString::number(color.red(),16))
                    .arg(QString::number(color.green(),16))
                    .arg(QString::number(color.blue(),16));
            tray->showMessage("Copied to the clipboard",str,QSystemTrayIcon::Information,1500);
            colorLabel->close();
            QClipboard *b = QApplication::clipboard();
            b->setText(str);
            this->hide();
        }
        break;
    case 2:
        break;
    }
}

void ScreenShot::mouseMoveEvent(QMouseEvent *e) //鼠标移动
{
    QPoint p = e->pos();
    QImage pic = bg;

    switch(choice){
    case 0:
        if(e->buttons() & Qt::LeftButton)
        {
            end = e->pos();
            pSize();
            rubber->setGeometry(px,py,pw,ph);
            setLabel(pw,ph,px,py);
            setButton(pw,ph,px,py);
        }
        break;
    case 1:
        color = pic.pixel(p.x(),p.y());
        setColorLabel(p.x(),p.y());
        break;
    case 2:
          ;
    }


}

void ScreenShot::mouseReleaseEvent(QMouseEvent *e) //鼠标松开
{

        if(e->button()==Qt::LeftButton)
        {

        }
        else if(e->button()==Qt::RightButton)
        {
            switch (choice) {
            case 0:
                if(quit == false)
                {
                    shot = true;
                    rubber->close();
                    label->close();
                    done->close();
                    quit = true;
                }
                else
                    this->hide();

                break;
            case 1:
                colorLabel->close();
                this->hide();
                break;
            }

        }
}

void ScreenShot::grabScreen() //抓取屏幕并截图保存
{
    QImage pic = bg.copy(px,py,pw,ph);
    QString path = QDir::currentPath()+"/"+QDateTime::currentDateTime().toString("yymmddhhmmss")+".jpg";
    pic.save(path);
    QDesktopServices::openUrl(QUrl(path));
    QClipboard *b = QApplication::clipboard();
    b->setImage(pic);
    done->close();
    label->close();
    rubber->close();
    this->hide();

}

void ScreenShot::setBackground(int w, int h,float n) //定格当前屏幕
{
    QScreen *screen = QGuiApplication::primaryScreen();
//    screen->grabWindow(0).save("bg.bmp","bmp");

    int r,g,b;
    bg = screen->grabWindow(0).toImage();
    QImage bg_grey(w,h,QImage::Format_RGB32);
    for(int i=0;i<w;i++)
    {
        for(int j=0;j<h;j++)
        {
            r = qRed(bg.pixel(i,j))*n;
            g = qGreen(bg.pixel(i,j))*n;
            b = qBlue(bg.pixel(i,j))*n;
            bg_grey.setPixel(i,j,qRgb(r,g,b));
        }
    }
    QPalette palette;
    palette.setBrush(this->backgroundRole(),QBrush(bg_grey));
    this->setPalette(palette);
    this->showFullScreen();
}

void ScreenShot::setLabel(int w,int h,int x,int y) //设置截图时显示尺寸的label
{
    QString size = QString("%1 x %2      ").arg(w).arg(h);
    label->setText(size);
    QRect rect(label->contentsRect());
    if(y>rect.height())
        label->move(QPoint(x,y-rect.height()));
    else
        label->move(QPoint(x,y));
    label->show();
}

void ScreenShot::setButton(int w,int h,int x,int y) //设置截图时确认保存的按钮
{
    QRect rect(done->contentsRect());
    if(height-y-h>rect.height())
        done->move(QPoint(x+w-rect.width(),y+h));
    else
        done->move(QPoint(x+w-rect.width(),y+h-rect.height()));
    done->show();
}

void ScreenShot::creatActions() //创建并关联托盘事件
{
    quitAction = new QAction("Quit",this);
    connect(quitAction,SIGNAL(triggered()),qApp,SLOT(quit()));
    pickAction = new QAction("pick color",this);
    connect(pickAction,SIGNAL(triggered()),this,SLOT(pickColor()));
//    gifAction = new QAction("make gif",this);
//    connect(gifAction,SIGNAL(triggered()),this,SLOT(makeGif()));
}

void ScreenShot::creatMenu() //创建托盘菜单
{
    trayIconMenu = new QMenu(this);
    trayIconMenu->addAction(pickAction);
//    trayIconMenu->addAction(gifAction);
    trayIconMenu->addAction(quitAction);
    tray->setContextMenu(trayIconMenu);
}

void ScreenShot::iconActivied(QSystemTrayIcon::ActivationReason reason)//创建托盘单击响应
{
    switch(reason)
    {
        case QSystemTrayIcon::Trigger: Shot(0.6);choice = 0;//choice为0为截屏
        break;
        default:break;
    }
}

void ScreenShot::Shot(float n) //截屏开始，初始化截屏时的控件，参数n为截屏后显示图片与原图的RGB之比
{
    QDesktopWidget *desktop = QApplication::desktop();
    QRect deskRect = desktop->screenGeometry();
    width = deskRect.width();
    height = deskRect.height();
    this->setMouseTracking(true);
    this->resize(width,height);
    this ->setWindowFlags(Qt::FramelessWindowHint|Qt::WindowStaysOnTopHint);
    shot = true;
    quit = true;
    setBackground(width,height,n);
    rubber =  NULL;
    origin = end = QPoint(0,0);
    label = new QLabel("");
    colorLabel = new QLabel("");
    done = new QPushButton("Done");
    label->setAttribute(Qt::WA_TranslucentBackground);
//    colorLabel->setAttribute(Qt::WA_TranslucentBackground);
    done->setAttribute(Qt::WA_TranslucentBackground);

    QPalette pal;
    pal.setColor(QPalette::WindowText,Qt::white);
    label->setPalette(pal);
//    colorLabel->setPalette(pal);
    done->setPalette(pal);

    label->setWindowFlags(Qt::FramelessWindowHint|Qt::WindowStaysOnTopHint);
    colorLabel->setWindowFlags(Qt::FramelessWindowHint|Qt::WindowStaysOnTopHint);
    done->setWindowFlags(Qt::FramelessWindowHint|Qt::WindowStaysOnTopHint);


    connect(done,SIGNAL(clicked()),this,SLOT(grabScreen()));
    this->show();
}

void ScreenShot::pickColor() //取色
{
    Shot(1);
    choice = 1; //choice为1为取色


}

//void ScreenShot::makeGif() //录GIF
//{
//    choice = 2; //choice为2为录gif
//}
