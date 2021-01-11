using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.UI;

public class QuickslotUI : MonoBehaviour
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
        public QuickslotData Data;
        public GameObject Prefab;
    }
    [SerializeField]
    private SlotInfo slotInfo;
    private List<RectTransform> slotList = new List<RectTransform>();

    private bool isActivable = true;

    private void Awake()
    {
        pannelRect.gameObject.SetActive( false );

        ObjectManager.Instance.OnChangeLocalPlayer += OnChangeLocalPlayer;
        ChatSystem.ChatEvent += OnChatEvent;

        UpdatePannelTransform();
        UpdateSlotTransform();
    }

    private void OnDestroy()
    {
        ObjectManager.Instance.OnChangeLocalPlayer -= OnChangeLocalPlayer;
        ChatSystem.ChatEvent -= OnChatEvent;
    }

    private void Update()
    {
        if ( Input.GetKeyDown( activeKey ) )
        {
            SetActivePannel( true );
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
                }
            }

            SetActivePannel( false );
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
        float slotInterval = ( 1.0f / slotInfo.Data.totalCount );

        // 중앙에서부터 원형으로 배치
        for ( int i = 0; i < slotInfo.Data.totalCount; ++i )
        {
            if ( slotList.Count <= i )
            {
                GameObject slot = Instantiate( slotInfo.Prefab, pannelRect );
                slotList.Add( slot.GetComponent<RectTransform>() );
            }

            RectTransform slotRect = slotList[ i ];

            // param = ( 0 ~ 2PI )
            float param = ( ( float )i / slotInfo.Data.totalCount ) * Mathf.PI * 2.0f;
            // ex: ( 0.125 ~ 0.875 ) = ( -1.0 ~ 1.0 ) * ( 1.0 - 0.25 ) + 1.0 * 0.5
            float anchorX = ( Mathf.Sin( param ) * ( 1.0f - slotInterval * 2.0f ) + 1.0f ) * 0.5f;
            float anchorY = ( Mathf.Cos( param ) * ( 1.0f - slotInterval * 2.0f ) + 1.0f ) * 0.5f;
            slotRect.anchorMin = new Vector2( anchorX - slotInterval, anchorY - slotInterval );
            slotRect.anchorMax = new Vector2( anchorX + slotInterval, anchorY + slotInterval );
        }
    }

    private void UpdateSlotData( Player _localPlayer )
    {
        if ( _localPlayer == null )
        {
            Debug.LogWarning( "localPlayer is null." );
            return;
        }

        foreach ( KeyValuePair<int/*index*/, Item> pair in slotInfo.Data.slotItems )
        {
            if ( pair.Key >= slotList.Count )
            {
                Debug.LogError( "invalid slot index. key = " + pair.Key + ", slotCount = " + slotList.Count );
                continue;
            }

            if ( ReferenceEquals( pair.Value, null ) )
            {
                continue;
            }

            RectTransform slotRect = slotList[ pair.Key ];

            Button buttonUI = slotRect.GetComponent<Button>();
            buttonUI.onClick.RemoveAllListeners();
            buttonUI.onClick.AddListener( () => 
            {
                pair.Value.OnUseItem( _localPlayer );
                SetActivePannel( false );
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

    private void SetActivePannel( bool _isActive )
    {
        if ( _isActive )
        {
            if ( !isActivable )
            {
                return;
            }

            pannelRect.gameObject.SetActive( true );
            Cursor.lockState = CursorLockMode.Confined;
            Cursor.visible = true;
        }
        else
        {
            pannelRect.gameObject.SetActive( false );
            Cursor.lockState = CursorLockMode.Locked;
        }
    }

    private void OnChangeLocalPlayer( Player _localPlayer )
    {
        UpdateSlotData( _localPlayer );
    }

    private void OnChatEvent( bool _isActive )
    {
        isActivable = !_isActive;
    }
}
