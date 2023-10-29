#pragma once
#ifndef __MUZI_MEVENT_H__
#define __MUZI_MEVENT_H__
#include<memory>
#include<functional>

namespace MUZI::_event
{
	class MEvent
	{
	public:
		enum EVENT_TYPE
		{
			// 鼠标事件
			CLICK = 1, // 单击
			DOUBLE_CLICK, // 双击
			DROP, // 拖拽
			RELEASE,  // 释放

			// 键盘事件
			KEY_BOARD, 

			// 信号触发事件
			SIGNAL_TRIGGER,
		};
		enum KEY_BOARD_TYPE
		{
			// 字母
			KEY_Q = 1001,
			KEY_W,
			KEY_E,
			KEY_R,
			KEY_T,
			KEY_Y,
			KEY_U,
			KEY_I,
			KEY_O,
			KEY_P,
			KEY_A,
			KEY_S,
			KEY_D,
			KEY_F,
			KEY_G,
			KEY_H,
			KEY_J,
			KEY_K,
			KEY_L,
			KEY_Z,
			KEY_X,
			KEY_C,
			KEY_V,
			KEY_B,
			KEY_N,
			KEY_M,

			// 操作键
			KEY_ENTER,
			KEY_SPACE,
			KEY_SHIFT,
			KEY_CTRL,
			KEY_CAPLOCK,
			KEY_TAB,
			KEY_BACK,
			KEY_INSERT,
			KEY_DELETE,
			KEY_END,
			KEY_PAGEUP,
			KEY_PAGEDOWN,

			//数字
			KEY_0,
			KEY_1,
			KEY_2,
			KEY_3,
			KEY_4,
			KEY_5,
			KEY_6,
			KEY_7,
			KEY_8,
			KEY_9,
			
			// 数字运算
			KEY_SLASH, // 斜杠 /
			KEY_STAR, // * 号 
			KEY_MINUS, // -号
			KEY_PLUSSIGN, // + 号
			KEY_EQUAL, // =号


		};

	public:
		int event_type;
		std::shared_ptr<void*> msg;

	};


}

#endif // !__MUZI_MEVENT_H__
