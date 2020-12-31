﻿using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class EquipQuickslot : MonoBehaviour
{
    public RectTransform pannelRect;
    public float pannelRatioAtScreen;

    [System.Serializable]
    public struct SlotInfo
    {
        public GameObject Prefab;
        public int VerticalCount;
        public int HorizontalCount;
    }
    public SlotInfo slotInfo;
    private List<RectTransform> slotList = new List<RectTransform>();

    private void Start()
    {
        UpdatePannelTransform();
        UpdateSlotTransform();
    }

    private void UpdatePannelTransform()
    {
        // 패널 크기를 정사각형으로 설정(짧은 축 기준)
        Vector2 anchorMin, anchorMax;
        if ( Screen.width < Screen.height )
        {
            anchorMin.x = 0.0f;
            anchorMin.y = 0.0f;
            anchorMax.x = pannelRatioAtScreen;
            anchorMax.y = ( pannelRatioAtScreen * Screen.width ) / Screen.height;
        }
        else
        {
            anchorMin.x = 0.0f;
            anchorMin.y = 0.0f;
            anchorMax.x = ( pannelRatioAtScreen * Screen.height ) / Screen.width;
            anchorMax.y = pannelRatioAtScreen;
        }

        pannelRect.anchorMin = anchorMin;
        pannelRect.anchorMax = anchorMax;
        pannelRect.localPosition = Vector3.zero;
    }

    private void UpdateSlotTransform()
    {
        int totalCount = slotInfo.VerticalCount * slotInfo.HorizontalCount;
        for ( int i = 0; i < totalCount; ++i )
        {
            if ( slotList.Count <= i )
            {
                GameObject slot = Instantiate( slotInfo.Prefab, pannelRect );
                slotList.Add( slot.GetComponent<RectTransform>() );
            }

            RectTransform slotRect = slotList[ i ];
            int vIndex = ( i % slotInfo.VerticalCount );
            int hIndex = ( i / slotInfo.VerticalCount );

            slotRect.anchorMin = new Vector2( vIndex / ( float )slotInfo.VerticalCount, hIndex / ( float )slotInfo.HorizontalCount );
            slotRect.anchorMax = new Vector2( ( vIndex + 1 ) / ( float )slotInfo.VerticalCount, ( hIndex + 1 ) / ( float )slotInfo.HorizontalCount );
        }
    }
}
