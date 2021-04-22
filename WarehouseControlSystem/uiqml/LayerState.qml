
import QtQuick 2.0

Item {
    Rectangle{
        id: root
        property int duration:3000
        property string layerName: "3"
        property string carName: "68"
        property  int  mState: 0
        width: 150//200
        height: 80
        color:"#1ce31d"
        Rectangle{
            id:road_top
            width: 150
            height: 5
            x:0
            y:root.height/3
            color:"#51413e"
            gradient: Gradient{
                GradientStop{position: 0.0 ;color :"#d3d0cf"}
                GradientStop{position: 1.0 ;color :"#51413e"}
            }
        }

        Rectangle{
            id:road_bottom
            width: 150
            height: 5
            x:0
            y:root.height/3*2
            color:"#51413e"
            gradient: Gradient{
                GradientStop{position: 0.0 ;color :"#d3d0cf"}
                GradientStop{position: 1.0 ;color :"#51413e"}
            }
        }


        Rectangle{
            id:road_mind
            width: 5
            height: road_bottom.y - road_top.y
            x:root.width/4
            y:road_top.y
            color:"#51413e"
            gradient: Gradient{
                GradientStop{position: 0.0 ;color :"#d3d0cf"}
                GradientStop{position: 1.0 ;color :"#51413e"}
            }
        }
        Rectangle{
            id:road_mind_1
            width: 5
            height: road_bottom.y - road_top.y
            x:root.width/4*2
            y:road_top.y
            color:"#51413e"
            gradient: Gradient{
                GradientStop{position: 0.0 ;color :"#d3d0cf"}
                GradientStop{position: 1.0 ;color :"#51413e"}
            }
        }
        Rectangle{
            id:road_mind_2
            width: 5
            height: road_bottom.y - road_top.y
            x:root.width/4*3
            y:road_top.y
            color:"#51413e"
            gradient: Gradient{
                GradientStop{position: 0.0 ;color :"#d3d0cf"}
                GradientStop{position: 1.0 ;color :"#51413e"}
            }
        }
        Rectangle{
            id:roller1
            width: 8
            height: 16
            //anchors.right: root.left
            x:root.width - 15
            y:root.height/4
            color:"#d3d0cf"
        }
        Rectangle{
            id:roller2
            width: 8
            height: 16
            //anchors.right: root.left
            x:root.width - 15
            y:root.height*3/4
            color:"#d3d0cf"
        }
        Rectangle{
            id:roller3
            width: 8
            height: 16
            //anchors.right: root.left
            x:0
            y:root.height/4
            color:"#d3d0cf"
        }
        Text {
            x:parent.width/2
            y:5
            font.family: "Helvetica"
            font.pointSize: 14
            color: "white"
            id: layerText
            text: qsTr(""+root.layerName)
        }
        Text {
            x:5
            y:parent.height-26
            font.family: "Helvetica"
            font.pointSize: 14
            color: "yellow"
            id: carText
            text: qsTr("Id:"+root.carName)
        }
//        Connections{
//            target: MainWindow
//            onLayerStateSignal:{carReadyIn(carNumber,state)}
//        }
        Connections{
            target:animOut
            onFinished:{
                car.visible = false;
                root.carName = "";
            }
        }

    }
    Image {
        id: car //width 50 height 50
        x:root.width
        y:root.height/3-2   //70
        source: "qrc:/resouse/Image/rgvcar.png"

        MouseArea{
            anchors.fill: car
            onClicked: {
                car.x = 150;y=40;//200
                animIn.restart()
                root.color = "#f36d58"
                //animOut.restart()
            }
        }

    }
    ParallelAnimation{
        id:animIn
        SequentialAnimation{

            NumberAnimation {
                target: car
                properties: "x"
                to: root.width/2-30
                duration: root.duration*0.6
                easing.type: Easing.OutCirc
            }
        }
    }

    ParallelAnimation{
        id:animOut
        SequentialAnimation{
            NumberAnimation {
                alwaysRunToEnd: false
                target: car
                properties: "y"
                to: 0
                duration: root.duration*0.8
                easing.type: Easing.OutCirc
            }
        }
    }
    function carReadyIn(carNumber,state,layerName){
        if(root.mState === state)
             return ;
        root.carName = carNumber;
        root.layerName = layerName;
        root.mState = state;
        if(state === 1) //free
        {
            root.color =  "#1ce31d"
            root.carName = carNumber;
            car.x =root.width;car.y=root.height/3-2;//
            carText.text = "";
        }
        if(state === 2) //car ready in layer
        {
            root.color =  "#f36d58"
            root.carName = carNumber;
        }
        else if(state === 3) //car inLayer
        {
            car.visible = true;
            car.x = root.width/2-car.width/2;car.y=root.height/3-2;//
            animIn.restart()
            root.color =  "#f36d58"
            carText.text = carNumber;
        }
        else if(state === 4)//car out
        {
            root.color = "#1ce31d"
            animOut.restart();
           root.carName = "";
           carText.text  = root.carName;

        }
    }
//    // 发送给 Qt Widgets 的信号
//    signal qmlSignal
//    // 从 Qt Widgets 接收到的信号
//    signal cSignal(var v1,var v2,var v3)
//    function testR (v1, v2,v3) {    //槽

//        console.log("re:" + v1)
//        console.log("re:" + v2)
//    }
//    onCSignal: {

//           root.color = "blue"
//           myText.text = "Call the qml signal handler"
//       }
//    Component.onCompleted: {     //建立连接
//        cSignal.connect(testR)     //信号与槽的连接方式一（方式二请看文首链接）
//    }
}
