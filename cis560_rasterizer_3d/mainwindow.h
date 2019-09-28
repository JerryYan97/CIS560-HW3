#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QImage>
#include <QGraphicsScene>
#include <polygon.h>
#include <rasterizer.h>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    void DisplayQImage(QImage &i);

    void keyPressEvent(QKeyEvent *e);

private slots:
    void on_actionLoad_Scene_triggered();

    void on_actionSave_Image_triggered();

    void on_actionEquilateral_Triangle_triggered();

    void on_actionQuit_Esc_triggered();

    void on_action_w_triggered();

    void on_action_s_triggered();

    void on_action_a_triggered();

    void on_action_d_triggered();

    void on_action_q_triggered();

    void on_action_e_triggered();

    void on_action_UpArrow_triggered();

    void on_action_DownArrow_triggered();

    void on_action_LeftArrow_triggered();

    void on_action_RightArrow_triggered();

    void on_action_z_triggered();

    void on_action_x_triggered();
private:
    Ui::MainWindow *ui;
    Polygon LoadOBJ(const QString &file, const QString &polyName);

    //This is used to display the QImage produced by RenderScene in the GUI
    QGraphicsScene graphics_scene;

    //This is the image rendered by your program when it loads a scene
    QImage rendered_image;

    //The instance of the Rasterizer used to render our scene
    Rasterizer rasterizer;

};

#endif // MAINWINDOW_H
