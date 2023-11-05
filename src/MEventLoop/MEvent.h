#pragma once
#ifndef __MUZI_MEVENT_H__
#define __MUZI_MEVENT_H__
#include<memory>
#include<functional>
#include<compare>

namespace MUZI::_event
{
	class MEvent
	{
	public:
		enum EVENT_TYPE
		{
			// ���¼�
			NULL_EVENT = 10001,

			// ����¼�
			MOUSE,

			// �����¼�
			KEY_BOARD, 

			// �źŴ����¼�
			SIGNAL_TRIGGER,
		};
		enum MOUSE_TYPE
		{
			// ����¼�
			CLICK = 1001, // ����
			DOUBLE_CLICK, // ˫��
			DROP, // ��ק
			HOLD, // ���²��ſ�
			RELEASE,  // �ͷ�
		};
		enum KEY_BOARD_TYPE
		{
			// ��ĸ
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

			// ������
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

			//����
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
			
			// ��������
			KEY_SLASH, // б�� /
			KEY_STAR, // * �� 
			KEY_MINUS, // -��
			KEY_PLUSSIGN, // + ��
			KEY_EQUAL, // =��


		};
	public:
		struct MouseEventMsg
		{
			int event_type;
			short abs_x, abs_y;
			short relative_x, relative_y;
		};
		struct KeyBoardEventMsg
		{
			int event_type;
		
		};
		struct SignalTriggerMsg
		{
			using SlotObj = void*;
			using SignalObj = void*;
			
		};
	public:
		MEvent();
	public:
		bool operator()(const MEvent& event);
		std::weak_ordering operator<=>(const MEvent& event);
		bool operator==(const MEvent& event);

		std::weak_ordering operator<=>(const MEvent& event) const;
		bool operator==(const MEvent& event) const;

	public:
		int m_event_type;
		std::shared_ptr<void*> m_event_msg;// �¼��ľ�����Ϣ����������¼���key_0 �ȵ�
		int m_priority;
	};


}

#endif // !__MUZI_MEVENT_H__
