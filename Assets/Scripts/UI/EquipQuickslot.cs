using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class EquipQuickslot : MonoBehaviour
{
    public RectTransform pannelRect;
    public float pannelRatioAtScreen;

    public GameObject slotPrefab;

    private void Start()
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
}
