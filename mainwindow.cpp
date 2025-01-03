#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <QFileDialog>//选择文件
#include <QDir>//获取内容
#include <QMediaPlayer>
#include <QSettings>

MainWindow::MainWindow(QWidget *parent):
    QMainWindow(parent),
    //主界面的构造函数
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    this->setWindowTitle("MyMusicPlayer");
    this->setMinimumSize(800,430);
    this->setMaximumSize(800,430);//限制长宽
    ui->stackedWidget->setCurrentIndex(0);

    //////////////////////////////
    //参数记忆设置
    QSettings setting("MyMusicPlayer");

    play_list_mode=setting.value("play_list_mode").toInt();
    current_volume=setting.value("current_volume").toFloat();
    current_file=setting.value("current_file").toString();
    play_list_index=setting.value("play_list_index").toInt();
    current_process=setting.value("current_process",current_process).toLongLong();
    if(!current_file.isEmpty()){
        load_file(current_file);
    }

    /////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //播放设置
    ui->switch_to_song->setDisabled(true);
    ui->start->setDisabled(true);
    ui->last->setDisabled(true);
    ui->next->setDisabled(true);
    ui->process_bar->setDisabled(true);
    player->setAudioOutput(output);
    output->setVolume(current_volume);
    connect(MainWindow::player, SIGNAL(mediaStatusChanged(QMediaPlayer::MediaStatus)), this, SLOT(set_song_info()));
    //// 音量条
    ui->volume_slider->setParent(this);
    ui->volume_slider->setRange(0,100);
    ui->volume_slider->setValue(current_volume*100.0);
    ui->volume_slider->setGeometry(635,345,150,15);
    ui->volume_slider->hide();
    ui->volume_dis->setAlignment(Qt::AlignCenter);
    ui->volume_dis->hide();
    ////////////////////////////////////////////////////////////////////////////////

    QString button_style="QPushButton { background-color: transparent; border: none; }"
                                 "QPushButton:hover {background-color: rgba(0, 0, 0, 0.1);}"
                                 "QPushButton:pressed { background-color: rgba(0, 0, 0, 0.05); }";

    ////进度条
    connect(player,&QMediaPlayer::durationChanged,this,[=](qint64 duration){
        ui->total_time->setText(QString("%1:%2").arg( QString::number(duration/60000),QString::number(duration%60000/1000).rightJustified(2,'0')) );
        ui->process_bar->setRange(0,duration);
    });
    connect(player,&QMediaPlayer::positionChanged,this,[=](qint64 pos){
        static int t=0;
        if (t>10)
        {
            ui->current_time->setText(QString("%1:%2").arg( QString::number(pos/60000),QString::number(pos%60000/1000).rightJustified(2,'0')) );
            ui->process_bar->setValue(pos);
            t=0;
        }
        // qInfo()<<t;
        t++;
    });
    ui->current_time->setAlignment(Qt::AlignCenter);
    ui->total_time->setAlignment(Qt::AlignCenter);

    ////to song_face button
    ui->song_face->setScaledContents(true);
    ui->music_pic->setScaledContents(true);
    ui->music_name->setAlignment(Qt::AlignLeft|Qt::AlignVCenter);
    ui->switch_to_song->setParent(this);
    ui->switch_to_song->setGeometry(10,349,210,75);
    ui->switch_to_song->raise();
    ui->switch_to_song->setStyleSheet(button_style);

    ///Button_icon
    //////////////////////////////////////////////////////////////////////////////////
    {
    if(play_status){ui->start->setIcon(pause_icon);}
    else{ui->start->setIcon(start_icon);}

    ui->start->setIconSize(QSize(52,52));
    ui->start->setStyleSheet(button_style);

    ui->last->setIcon(last_icon);
    ui->last->setIconSize(QSize(42,42));
    ui->last->setStyleSheet(button_style);

    ui->next->setIcon(next_icon);
    ui->next->setIconSize(QSize(42,42));
    ui->next->setStyleSheet(button_style);

    switch(play_list_mode){
    case 0:
        ui->mode->setIcon(order_icon);
        break;
    case 1:
        ui->mode->setIcon(loop_icon);
        break;
    case 2:
        ui->mode->setIcon(single_icon);
        break;
    case 3:
        ui->mode->setIcon(random_icon);
        break;
    }

    ui->mode->setIconSize(QSize(28,28));
    ui->mode->setStyleSheet(button_style);

    ui->list->setIcon(list_icon);
    ui->list->setIconSize(QSize(28,28));
    ui->list->setStyleSheet(button_style);

    if(current_volume){ui->volume->setIcon(volume_icon);}
    else{ui->volume->setIcon(mute_icon);}
    ui->volume->setIconSize(QSize(28,28));
    ui->volume->setStyleSheet(button_style);

    ui->to_config->setIcon(config_icon);
    ui->to_config->setIconSize(QSize(32,32));
    ui->to_config->setStyleSheet(button_style);

    ui->back_to_main->setIcon(back_icon);
    ui->back_to_main->setIconSize(QSize(32,32));
    ui->back_to_main->setStyleSheet(button_style);

    }
    //////////////////////////////////////////////////////////////////////////////////

}

MainWindow::~MainWindow(){
    delete ui;
    QSettings setting("MyMusicPlayer");

    setting.setValue("play_list_mode",play_list_mode);
    setting.setValue("current_volume",current_volume);
    setting.setValue("current_file",current_file);
    setting.setValue("play_list_index",play_list_index);
    current_process=player->position();
    setting.setValue("current_process",current_process);
}
///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

//歌曲界面
void MainWindow::on_switch_to_song_clicked(){
    if(ui->stackedWidget->currentIndex()!=1){
        ui->stackedWidget->setCurrentIndex(1);
        setWindowOpacity(0.98);
    }
    else{
        ui->stackedWidget->setCurrentIndex(0);
        setWindowOpacity(1.0);
    }
}

//播放按钮
void MainWindow::on_start_clicked()
{
    if(play_status){  //播放时
        ui->start->setIcon(start_icon);
        play_status=false;
        player->pause();

    }
    else{  //暂停时
        ui->start->setIcon(pause_icon);
        play_status=true;
        player->play();
    }
}

//播放模式
void MainWindow::on_mode_clicked()
{
    play_list_mode+=1;
    if(play_list_mode>3){play_list_mode=0;}
    switch(play_list_mode){
        case 0:
            ui->mode->setIcon(order_icon);
            break;
        case 1:
            ui->mode->setIcon(loop_icon);
            break;
        case 2:
            ui->mode->setIcon(single_icon);
            break;
        case 3:
            ui->mode->setIcon(random_icon);
            break;
    }

}
void MainWindow::on_list_clicked(){
    ui->stackedWidget->setCurrentIndex(0);
    setWindowOpacity(1.0);
}
//进度条
void MainWindow::on_process_bar_valueChanged(int value){
    if(player->duration()-value>300){ //防止拖到尾部连续切歌
        player->setPosition(value);}
    else{
        player->setPosition(player->duration()-300);
    }
}

//设置
void MainWindow::on_to_config_clicked(){
        ui->stackedWidget->setCurrentIndex(2);
}

void MainWindow::on_back_to_main_clicked(){
    ui->stackedWidget->setCurrentIndex(0);
}
//音量调整
////////////////////////////////////////////////////////////////////////////////
void MainWindow::on_volume_clicked()
{
    if(ui->volume_slider->isHidden()){
        ui->volume_slider->show();
        ui->volume_dis->setText(QString::number(current_volume*100.0));
        ui->volume_dis->show();
    }
    else{
        ui->volume_slider->hide();
        ui->volume_dis->hide();
    }
}
void MainWindow::update_volume_icon(){
    if(current_volume){
        ui->volume->setIcon(volume_icon);
    }
    else{
        ui->volume->setIcon(mute_icon);
    }
}
void MainWindow::on_volume_slider_valueChanged(int value)
{
    current_volume=value/100.0;
    output->setVolume(current_volume);
    ui->volume_dis->setText(QString::number(current_volume*100.0));
    update_volume_icon();

}
////////////////////////////////////////////////////////////////////////////////


//歌曲信息
//////////////////////////////////////////////////////////////////////////////////////////
////圆角图
QPixmap MainWindow::set_border_radius(QPixmap cover,int radius){
    QBitmap mask(cover.size());
    mask.fill(Qt::color0);
    QPainter painter(&mask);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setBrush(Qt::color1);
    painter.drawRoundedRect(mask.rect(), radius, radius);
    cover.setMask(mask);
    return cover;
}
////
////歌曲更新
void MainWindow::set_song_info(){
    if(player->isAvailable()){
        static bool when_launch=true;
        if(current_process!=0){
            if(when_launch){
                player->setPosition(current_process);
                ui->process_bar->setValue(current_process);
                when_launch=false;
            }
        }
        ////封面和标题
        if(player->mediaStatus()==QMediaPlayer::LoadedMedia){
            QMediaMetaData mediaData = player->metaData();

            current_name  = mediaData.value(QMediaMetaData::Title).toString();
            if(!current_name.isEmpty()){
                ui->music_name->setText(current_name);
            }
            else{
                ui->music_name->setText(player->source().fileName());
            }

            QVariant coverdata1 = mediaData.value(QMediaMetaData::CoverArtImage);
            QVariant coverdata2 = mediaData.value(QMediaMetaData::ThumbnailImage);
            ui->song_face->show();
            if(!coverdata1.isNull()){
                current_cover = qvariant_cast<QPixmap>(coverdata1);
                ui->music_pic->setPixmap(set_border_radius(current_cover,30));
                ui->song_face->setPixmap(set_border_radius(current_cover,20));
            }
            else if(!coverdata2.isNull()){
                current_cover = qvariant_cast<QPixmap>(coverdata2);
                ui->music_pic->setPixmap(set_border_radius(current_cover,30));
                ui->song_face->setPixmap(set_border_radius(current_cover,20));
            }
            else{
                ui->music_pic->setStyleSheet("background:#FFFFFF;");
                ui->music_pic->setText("No Image");
                ui->song_face->hide();
            }



            ui->switch_to_song->setDisabled(false);
            ui->start->setDisabled(false);
            ui->last->setDisabled(false);
            ui->next->setDisabled(false);
            ui->process_bar->setDisabled(false);
        }
        ////切歌
        if(player->mediaStatus()==QMediaPlayer::EndOfMedia){
            if(play_status){
                if(play_list_mode==0){ //order
                    if(play_list_index<play_list.length()-1){
                        play_list_index+=1;
                        update_song_status();
                    }
                    else{
                        player->pause();
                        ui->start->setIcon(start_icon);
                    }
                }
                else if(play_list_mode==2){//single
                    update_song_status();
                }
                else{
                    on_next_clicked();
                }

            }

        }
    }

}
//////////////////////////////////////////////////////////////////////////////////////////

//选择文件
void MainWindow::load_file(QString filepath){
    current_file=filepath;  //更新路径
    QDir dir(filepath);     //获取歌曲信息
    auto music_list=dir.entryList(QStringList()<<"*.mp3"<<"*.wav"<<"*.flac");

    //播放列表操作
    //1.将歌曲添加入播放列表
    int num=1;
    for(auto &music:music_list){
        if(music.endsWith(".mp3")|music.endsWith(".wav")|music.endsWith(".flac")){
            ui->listWidget->addItem(QString::number(num)+"          "+music);
            play_list.append(QUrl::fromLocalFile(filepath+"/"+music));
            num++;
        }
    }
    if(!play_list.isEmpty()){
        //2.载入当前歌曲
        if(play_list_index>play_list.length()-1 ||play_list_index<0){play_list_index=0;}//防止意外修改

        ui->listWidget->setCurrentRow(play_list_index);
        play_list_index=ui->listWidget->currentRow();

        //3.设置播放源
        player->setSource(play_list[play_list_index]);
        if(play_status){
            player->play();}
    }
}
void MainWindow::on_choose_file_clicked(){
    QString filepath=QFileDialog::getExistingDirectory(this,QString::fromLocal8Bit("选择音乐文件夹"),
                    QCoreApplication::applicationFilePath());
    if(!filepath.isEmpty()){
        current_file=filepath;
        load_file(filepath);
    }

}

//手动歌曲切换
//////////////////////////////////////////////////////////////////////////////////////////
void MainWindow::update_song_status(){
    ui->listWidget->setCurrentRow(play_list_index);
    player->setSource(play_list[play_list_index]);
    if(play_status){player->play();}
}
void MainWindow::on_last_clicked(){ //0-order;1-loop;2-single;3-random;
    switch(play_list_mode){
    case 0:
        if(play_list_index>0){
            play_list_index-=1;
        }
        break;
    case 1:
        if(play_list_index>0){
            play_list_index-=1;
        }
        else{
            play_list_index=play_list.length()-1;
        }
        break;
    case 2:
        if(play_list_index>0){
            play_list_index-=1;
        }
        else{
            play_list_index=play_list.length()-1;
        }
        break;
    case 3:
        QRandomGenerator *ge = QRandomGenerator::global();
        play_list_index=ge->bounded(0,play_list.length());
        break;
    }
    update_song_status();

}
void MainWindow::on_next_clicked(){
    switch(play_list_mode){
    case 0:
        if(play_list_index<play_list.length()-1){
            play_list_index+=1;
        }
        break;
    case 1:
        if(play_list_index<play_list.length()-1){
            play_list_index+=1;
        }
        else{
            play_list_index=0;
        }
        break;
    case 2:
        if(play_list_index<play_list.length()-1){
            play_list_index+=1;
        }
        else{
            play_list_index=0;
        }
        break;
    case 3:
        QRandomGenerator *ge = QRandomGenerator::global();
        play_list_index=ge->bounded(0,play_list.length());
        break;
    }
    update_song_status();
}
void MainWindow::on_listWidget_itemDoubleClicked(QListWidgetItem *item)
{
    play_list_index=ui->listWidget->row(item);
    play_status=true;
    ui->start->setIcon(pause_icon);
    update_song_status();
}
//////////////////////////////////////////////////////////////////////////////////////////

void MainWindow::on_clear_clicked()
{
    player->setSource(QUrl());
    current_file="";
    play_list.clear();
    play_list_index=0;
    ui->listWidget->clear();
    set_song_info();
    ui->start->setDisabled(true);
    ui->last->setDisabled(true);
    ui->next->setDisabled(true);
    ui->music_name->clear();
    ui->music_pic->clear();
    ui->switch_to_song->setDisabled(true);
    ui->current_time->clear();
    ui->total_time->clear();

}

