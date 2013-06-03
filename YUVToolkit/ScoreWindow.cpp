﻿#include "YT_InterfaceImpl.h"
#include "ScoreWindow.h"


ScoreWindow::ScoreWindow( QWidget *parent, Qt::WFlags flags ) : QWidget(parent, flags)
{
	ui.setupUi(this);

	connect(ui.buttonNext, SIGNAL(clicked()), this, SIGNAL(onNext()));
	connect(ui.buttonPrevious, SIGNAL(clicked()), this, SIGNAL(onPrevious()));
	connect(ui.buttonFinish, SIGNAL(clicked()), this, SIGNAL(onFinish()));
}

ScoreWindow::~ScoreWindow()
{

}

void ScoreWindow::enableButtons( bool p, bool n, bool f)
{
	ui.buttonPrevious->setEnabled(p);
	ui.buttonNext->setEnabled(n);
	ui.buttonFinish->setEnabled(f);
}


void ScoreWindow::changeButtonName(QString a, QString b, QString c)
{
	ui.buttonPrevious ->setText(a);
	ui.buttonNext->setText(b);
	ui.buttonFinish->setText(c);
}

void ScoreWindow::createButton(int num,QString name)
{	
	ButtonNameList = name.split(',');
	QGridLayout *box = new QGridLayout;
	ButtonList = (QRadioButton **) malloc (num * sizeof(QRadioButton));
	NumOfConditions = num;
	QLabel * label_static = new QLabel("What you choose is");
	label_cur_selected = new QLabel("Not_rated");
	box->addWidget(label_static,0,0);
	box->addWidget(label_cur_selected,0,1);
	for (int i=0;i<num;i++)
	{
		QRadioButton *button = new QRadioButton(ButtonNameList[i]);
		box->addWidget(button,1,i);
		connect(button,SIGNAL(clicked()),this,SLOT(updateCurSelect()));
		ButtonList[i] = button;
	};
	ui.widget_score->setLayout(box);
	ui.widget_score->show();
}

void ScoreWindow::createSlider(int num,int min, int max, QString name, QString scale)
{
	SliderNameList = name.split(',');
	ScaleNameList = scale.split(',');

	QGridLayout *box = new QGridLayout;
	SliderList = (QSlider **) malloc (num * sizeof (QSlider));
	label_cur_selected_list = (QLabel **) malloc (num * sizeof(QLabel));
	NumOfConditions = num;
	box->setAlignment(Qt::AlignHCenter);
	for (int i=0;i<num;i++)
	{
		QWidget *scale_widget = new QWidget;
		QVBoxLayout *scale_box = new QVBoxLayout;
		for (int j=0;j<ScaleNameList.size();j++)
		{
			QLabel* label_cur = new QLabel(ScaleNameList[j]);
			scale_box->addWidget(label_cur);
		}
		scale_widget->setLayout(scale_box);
		QLabel* label_cur = new QLabel("Not_rated");
		QSlider* slider= new QSlider();
		slider->setObjectName(SliderNameList[i]);
		slider->setRange(min, max);
		slider->setTickPosition(QSlider::TicksRight);
		slider->setTickInterval((max-min)/ScaleNameList.size());
		box->addWidget(label_cur,0,i*2);
		box->addWidget(slider,1,i*2);
		box->addWidget(scale_widget,1,i*2+1);
		connect(slider,SIGNAL(valueChanged(int)),label_cur,SLOT(setNum(int)));
		connect(slider,SIGNAL(valueChanged(int)),this,SLOT(updateCurSlider()));
		SliderList[i] = slider;
		label_cur_selected_list[i] = label_cur;
	};
	ui.widget_score->setLayout(box);
	ui.widget_score->show();
}

void ScoreWindow::updateCurSelect()
{
	for (int i=0; i < NumOfConditions; i++)
	{
		if (ButtonList[i]->isChecked() == true)
		{
				label_cur_selected->setText(ButtonList[i]->text());
				ui.buttonNext->setEnabled(true);
		}
	};
}

void ScoreWindow::updateCurSlider()
{
	bool enable = true;
	for (int i=0; i < NumOfConditions; i++)
	{
		QString SingleResult;
		QString cur_label = label_cur_selected_list[i]->text();
		if (cur_label.contains("Not_rated"))
		{
			enable = false;
		}		
	};
	if (enable)
		ui.buttonNext->setEnabled(true);
}

void ScoreWindow::initButton()
{
	for (int i=0; i < NumOfConditions; i++)
	{
		ButtonList[i]->setAutoExclusive(false);
		ButtonList[i]->setChecked(false);
		ButtonList[i]->setAutoExclusive(true);
	};
	label_cur_selected->setText("Not_rated");
}

void ScoreWindow::initSlider()
{
	for (int i=0; i < NumOfConditions; i++)
		{
			SliderList[i]->setValue(0);
			label_cur_selected_list[i]->setText("Not_rated");
	}
}

QString ScoreWindow::getCurSliderResults(const QStringList& fileList)
{
	QString Results;
	Results.append("{");
	for (int i=0; i < NumOfConditions; i++)
	{
		QString SingleResult;
		const QString& fName = fileList[i];
		QString cur_label = label_cur_selected_list[i]->text();	
		SingleResult = "\"" + fName + "\":" + cur_label; 
		if (i != NumOfConditions -1)
		{
			SingleResult.append(",");
		};
		Results.append(SingleResult);
	};
	Results.append("}\n");
	return Results;
}

QString ScoreWindow::getCurButtonResults(const QStringList& fileList)
{
	QString Results;
	Results.append("{");
	for (int i=0; i < NumOfConditions; i++)
	{
		QString SingleResult;
		const QString& fName = fileList[i];
		if (ButtonList[i]->isChecked() == true)
		{		
			SingleResult = "\"" + fName + "\":1"; 
		}
		else
		{
			SingleResult = "\"" + fName + "\":0"; 
		};
		if (i != NumOfConditions -1)
		{
			SingleResult.append(",");
		};
		Results.append(SingleResult);
	};
	Results.append("}\n");
	return Results;
}

void ScoreWindow::openResultsFile(QString fname, int mode)
{
	FileHandle = new QFile(fname);
	switch(mode) {
	case 0: FileHandle->open(QIODevice::WriteOnly | QIODevice::Text); break;
	case 1: FileHandle->open(QIODevice::ReadOnly | QIODevice::Text); break;
	case 2: FileHandle->open(QIODevice::Append | QIODevice::Text); break;
	default:
		break;
	}
}

void ScoreWindow::writeResultsFile(QString results)
{
	QTextStream out(FileHandle);
	out<<results;
	out.flush();
}

void ScoreWindow::closeResultsFile()
{
	FileHandle->close();
}

#ifdef SHUFFLE

void objFromScriptValue(const QScriptValue& obj, SceneVideo& s)
{
	int len = obj.property("length").toInteger(); 
	for(int i =0; i< len; i ++)
	{
		QStringList ql = obj.property(i).toVariant().toStringList();
		s.append(ql);
	}
}

QScriptValue ScriptValueFromobj(QScriptEngine* eng, const SceneVideo& s)
{
	QScriptValue a = eng->newObject();
	int len=s.size();
	a.setProperty("length",len);
	for(int i=0; i< len; i ++)
	{
		QScriptValue temp;
		int len_s = s[i].size();
		temp.setProperty("length",len_s);
		for(int j=0; j< len_s; j ++)
		{
			temp.setProperty(quint32(j), s[i][j]);
		}
		a.setProperty(quint32(i),temp);
	}
	return a;
}

void ScoreWindow::regMetaType()
{
	qScriptRegisterMetaType<SceneVideo>(&engine, ScriptValueFromobj,objFromScriptValue);
}

void ScoreWindow::shuffleList(SceneVideo origin, bool shuffle_scene, bool shuffle_video)
{
	if (shuffle_scene == true)
		std::random_shuffle(origin.begin(),origin.end());
	if (shuffle_video == true)
		{
			int len = origin.size();
			for (int i = 0; i< len; i++)
			{
				std::random_shuffle(origin[i].begin(),origin[i].end());
			}
	}
	return origin;	
}
#endif