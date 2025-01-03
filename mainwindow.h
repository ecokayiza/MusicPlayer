#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include<QPushButton>

#include <QtMultimedia/QMediaPlayer>//����
#include <QtMultimedia/QMediaMetaData>
#include <QtMultimedia/QAudioOutput>
#include <QListWidgetItem>
#include <QUrl>          //�����б�
#include <QRandomGenerator>
#include <QImage>
#include <QPainter>
#include <QFrame>
QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE
//��������Ķ���
class MainWindow : public QMainWindow
{
    Q_OBJECT //ʹ��qtԪ����
public: //�����������������
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
private:
    Ui::MainWindow *ui;  //uiָ��
/////////////////////////////////////////////////
private slots:  //�ۺ���������
    void on_switch_to_song_clicked();
    void on_start_clicked();
    void on_mode_clicked();
    void on_volume_clicked();
    void on_to_config_clicked();
    void on_back_to_main_clicked();
    void on_volume_slider_valueChanged(int value);
    void on_choose_file_clicked();
    void on_last_clicked();
    void on_next_clicked();
    void on_listWidget_itemDoubleClicked(QListWidgetItem *item);
    void update_song_status();
    void set_song_info();
    void on_process_bar_valueChanged(int value);
    void on_list_clicked();
    QPixmap set_border_radius(QPixmap cover,int radius);
    void on_clear_clicked();

public:  //����������Ҫ�õ��ı���������
    //////icons
    //////////////////////////////////////////////////////

    QIcon start_icon=QIcon(":/assets/play.png");
    QIcon pause_icon=QIcon(":/assets/pause.png");
    QIcon last_icon=QIcon(":/assets/left.png");
    QIcon next_icon=QIcon(":/assets/right.png");
    QIcon order_icon=QIcon(":/assets/order.png");
    QIcon loop_icon=QIcon(":/assets/loop.png");
    QIcon single_icon=QIcon(":/assets/single.png");
    QIcon random_icon=QIcon(":/assets/random.png");
    QIcon list_icon=QIcon(":/assets/list.png");
    QIcon volume_icon=QIcon(":/assets/volume.png");
    QIcon mute_icon=QIcon(":/assets/mute.png");
    QIcon config_icon=QIcon(":/assets/config.png");
    QIcon back_icon=QIcon(":/assets/back.png");

    //////////////////////////////////////////////////////
    void update_volume_icon();
    void load_file(QString filepath);

    QMediaPlayer *player=new QMediaPlayer(this);
    QAudioOutput *output=new QAudioOutput(this);
    bool play_status=false;
    QString current_name;     //��ǰ����
    QPixmap current_cover;
    QList<QUrl> play_list;    //�����б�
//����
//////////////////////////////////////////////////

    int play_list_mode=0; //0-order;1-loop;2-single;3-random;
    float current_volume=0.3;
    QString current_file="";  //��ǰ�ļ���
    int play_list_index=0;
    qint64 current_process=0;
    // QPixmap background_pic;

////////////////////////////////////////////////
};



#endif // MAINWINDOW_H
