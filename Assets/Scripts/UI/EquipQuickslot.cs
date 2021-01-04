using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.UI;

public class EquipQuickslot : MonoBehaviour
{
    [SerializeField]
    private KeyCode activeKey;

    [SerializeField]
    private RectTransform pannelRect;
    [SerializeField]
    private float pannelRatioAtScreen;

    [System.Serializable]
    public struct SlotInfo
    {
        public GameObject Prefab;
        public int TotalCount;
    }
    [SerializeField]
    private SlotInfo slotInfo;
    private List<RectTransform> slotList = new List<RectTransform>();

    private void Awake()
    {
        pannelRect.gameObject.SetActive( false );

        SceneBase.Instance.OnChangeLocalPlayer += OnChangeLocalPlayer;
    }

    private void Start()
    {
        UpdatePannelTransform();
        UpdateSlotTransform();
    }

    private void Update()
    {
        if ( Input.GetKeyDown( activeKey ) )
        {
            pannelRect.gameObject.SetActive( true );
            Cursor.lockState = CursorLockMode.Confined;
            Cursor.visible = true;
        }
        else if ( Input.GetKeyUp( activeKey ) && pannelRect.gameObject.activeSelf )
        {
            float distance = Vector2.Distance( Input.mousePosition, pannelRect.position );
            if ( distance > pannelRect.rect.width * 0.25f )
            {
                // 마우스에 가까운 퀵슬롯 사용
                int slotIndex = GetNearestSlotIndex();
                if ( slotIndex >= 0 && slotIndex < slotList.Count )
                {
                    Button buttonUI = slotList[ slotIndex ].GetComponent<Button>();
                    buttonUI.onClick?.Invoke();

                    return;
                }
            }

            pannelRect.gameObject.SetActive( false );
            Cursor.lockState = CursorLockMode.Locked;
        }
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
        float slotInterval = ( 1.0f / slotInfo.TotalCount );

        // 중앙에서부터 원형으로 배치
        for ( int i = 0; i < slotInfo.TotalCount; ++i )
        {
            if ( slotList.Count <= i )
            {
                GameObject slot = Instantiate( slotInfo.Prefab, pannelRect );
                slotList.Add( slot.GetComponent<RectTransform>() );
            }

            RectTransform slotRect = slotList[ i ];

            // param = ( 0 ~ 2PI )
            float param = ( ( float )i / slotInfo.TotalCount ) * Mathf.PI * 2.0f;
            // ( -1 ~ 1 ) => ( 0 ~ 1 )
            float anchorX = ( Mathf.Sin( param ) + 1.0f ) * 0.5f;
            float anchorY = ( Mathf.Cos( param ) + 1.0f ) * 0.5f;
            slotRect.anchorMin = new Vector2( anchorX - slotInterval, anchorY - slotInterval );
            slotRect.anchorMax = new Vector2( anchorX + slotInterval, anchorY + slotInterval );
        }
    }

    private void UpdateSlotData( Player localPlayer )
    {
        if ( localPlayer == null )
        {
            Debug.LogWarning( "localPlayer is null." );
            return;
        }

        foreach ( KeyValuePair<int/*index*/, Equipment> pair in localPlayer.equipQuickslot )
        {
            if ( pair.Key >= slotList.Count )
            {
                Debug.LogError( "invalid slot index. key = " + pair.Key + ", slotCount = " + slotList.Count );
                continue;
            }

            RectTransform slotRect = slotList[ pair.Key ];

            Button buttonUI = slotRect.GetComponent<Button>();
            buttonUI.onClick.AddListener( () => 
            {
                localPlayer.UseEquipQuickslot( pair.Key );

                pannelRect.gameObject.SetActive( false );
                Cursor.lockState = CursorLockMode.Locked;
            } );

            Image imageUI = slotRect.GetComponent<Image>();
            imageUI.sprite = pair.Value.icon;

            Text textUI = slotRect.GetComponentInChildren<Text>();
            textUI.text = pair.Value.id;
        }
    }

    private int GetNearestSlotIndex()
    {
        int nearestSlotIndex = -1;
        float nearestSlotDistance = float.MaxValue;

        for ( int i = 0; i < slotList.Count; ++i )
        {
            float distance = Vector2.Distance( slotList[ i ].position, Input.mousePosition );
            if ( distance < nearestSlotDistance )
            {
                nearestSlotDistance = distance;
                nearestSlotIndex = i;
            }
        }

        return nearestSlotIndex;
    }

    private void OnChangeLocalPlayer( Player localPlayer )
    {
        UpdateSlotData( localPlayer );
    }
}
