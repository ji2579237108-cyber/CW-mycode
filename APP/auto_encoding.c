//#include "auto_encoding.h"

//static void AutoAddressingEnter(STR_CAN_STD_TYPE *ptr)
//{   
//    ptr->canid.exdid = 0x1AD5F5F4 ;
//    ptr->data[0] = 0xCE;
//    ptr->data[1] = 0x20;
//    ptr->data[2] = 0x01;
//    ptr->len = 3;
//    InCan1Send(&canf);
//}
//static void AutoAddressingIdSet(STR_CAN_STD_TYPE *ptr)
//{   
//    int epass = 0;
//}

//BOOL CanAutoCodeHandler(STR_CAN_STD_TYPE *ptr)
//{
//    STR_CAN_STD_TYPE CanMsg = *ptr;

//    UINT8 RecId = (UINT8)(CanMsg.canid.exdid>>8);
//    
//    if (ptr->J1939PRDP != 0x1A || ptr->J1939PS != 0xF5) {
//            return FALSE;
//    }

//    InCan1Send(ptr);
//    

//    switch(((ptr->data[0]<<8)+ptr->data[1]))
//    {
//    case ADDR_MODE_ENTER:

//        AutoAddressingEnter(&CanMsg);
//    
//        break;

//    case ADDR_SET:
//        
//        AutoAddressingIdSet(&CanMsg);

//        break;
//    case TRANS_TOKEN:
//        
//        AutoAddressingTransToken(&CanMsg);

//        break;
//    default:
//        break;
//    }
//}

