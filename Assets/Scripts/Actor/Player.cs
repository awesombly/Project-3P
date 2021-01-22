using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.ResourceManagement.AsyncOperations;

public enum EBoneType
{
    Root,
    LeftHand,
    RightHand,
}

public class Player : Character
{
    #region Equipment
    [System.Serializable]
    public struct BoneInfo
    {
        // 자식 추가시 원본 크기 설정용
        static public Vector3 OriginalScale;

        public EBoneType BoneType;
        public Transform Reference;
    }
    [SerializeField]
    private List<BoneInfo> boneList; // 인스펙터 편집용
    private Dictionary<EBoneType, BoneInfo> boneInfos = new Dictionary<EBoneType, BoneInfo>();

    private struct EquipInfo
    {
        public Equipment Equip;
        public List<GameObject> Models;
    }
    private Dictionary<EEquipType, EquipInfo> equipInfos = new Dictionary<EEquipType, EquipInfo>();
    public delegate void DelChangeEquipment( Equipment _equip );
    public event DelChangeEquipment OnChangeEquipment;
    #endregion

    #region State
    private bool isSprinting = true;
    internal bool IsSprinting
    {
        get { return isSprinting; }
        set
        {
            if ( isSprinting == value )
            {
                return;
            }

            isSprinting = value;
            animator.SetBool( AnimatorParameters.IsSprinting, isSprinting );
        }
    }

    private bool isGrounded = true;
    internal bool IsGrounded
    {
        get { return isGrounded; }
        set
        {
            if ( isGrounded == value )
            {
                return;
            }

            isGrounded = value;
            animator.SetBool( AnimatorParameters.IsGrounded, isGrounded );

            OnChangeGrounded?.Invoke( isGrounded );
        }
    }
    public delegate void DelChangeGrounded( bool _isGrounded );
    public event DelChangeGrounded OnChangeGrounded;

    private bool isCrouching = false;
    internal bool IsCrouching
    {
        get { return isCrouching; }
        set
        {
            if ( isCrouching == value )
            {
                return;
            }

            isCrouching = value;
            animator.SetBool( AnimatorParameters.IsCrouching, isCrouching );

            OnChangeCrouching?.Invoke( isCrouching );
        }
    }
    public delegate void DelChangeCrouching( bool _isCrouching );
    public event DelChangeCrouching OnChangeCrouching;

    private bool isStrafing = true;
    internal bool IsStrafing
    {
        get { return isStrafing; }
        set
        {
            if ( isStrafing == value )
            {
                return;
            }

            isStrafing = value;
            animator.SetBool( AnimatorParameters.IsStrafing, isStrafing );

            OnChangeStrafing?.Invoke( isStrafing );
        }
    }
    public delegate void DelChangeStrafing( bool _isStrafing );
    public event DelChangeStrafing OnChangeStrafing;
    #endregion

    [System.Serializable]
    public struct FocusInfo
    {
        public LayerMask CullingLayer;
        public float RayDistance;
        public KeyCode InteractionKey;
        internal Actor Target;
    }
    public FocusInfo focusInfo;

    public delegate void DelChangeFocusTarget( Actor _target );
    public static event DelChangeFocusTarget OnChangeFocusTarget;

    protected override void Awake()
    {
        base.Awake();

        OnChangeCrouching += SendSyncCrouch;
        OnChangeGrounded += SendSyncGrounded;

        animator.SetBool( AnimatorParameters.IsStrafing, isStrafing );

        foreach ( BoneInfo info in boneList )
        {
            boneInfos.Add( info.BoneType, info );
        }

        Vector3 rootScale = boneInfos[ EBoneType.Root ].Reference.lossyScale;
        BoneInfo.OriginalScale = new Vector3( 1.0f / rootScale.x, 1.0f / rootScale.y, 1.0f / rootScale.z );
    }

    protected override void FixedUpdate()
    {
        base.FixedUpdate();

        UpdateCrouchState();

        UpdateInteraction();
    }

    #region Equipment
    public void SetEquipment( Equipment _equip )
    {
        if ( _equip == null )
        {
            Debug.LogWarning( "equip is null." );
            return;
        }

        ReleaseEquipment( _equip.equipType, false );

        EquipInfo equipInfo;
        equipInfo.Equip = _equip;
        equipInfo.Models = new List<GameObject>();

        foreach ( Equipment.ModelInfo info in _equip.modelInfos )
        {
            GameObject instance = Instantiate( info.Prefab, boneInfos[ info.AttachBone ].Reference );

            MeshCollider mesh = instance.GetComponent<MeshCollider>();
            if ( !ReferenceEquals( mesh, null ) )
            {
                mesh.convex = true;
                mesh.isTrigger = true;
            }

            instance.transform.localScale = BoneInfo.OriginalScale;

            equipInfo.Models.Add( instance );
        }

        equipInfos.Add( _equip.equipType, equipInfo );
        OnChangeEquipment?.Invoke( _equip );
    }

    public void ReleaseEquipment( EEquipType _equipType, bool _invokeEvent )
    {
        if ( !equipInfos.ContainsKey( _equipType ) )
        {
            return;
        }

        EquipInfo prevEquip = equipInfos[ _equipType ];
        foreach ( GameObject model in prevEquip.Models )
        {
            Destroy( model );
        }
        equipInfos.Remove( _equipType );

        if ( _invokeEvent )
        {
            OnChangeEquipment?.Invoke( null );
        }
    }
    #endregion

    protected override void UpdateInputParameters()
    {
        base.UpdateInputParameters();

        float sprintingRate = ( isSprinting ? 1.5f : 1.0f );
        inputVertical *= sprintingRate;
        inputHorizontal *= sprintingRate;
    }

    protected override void UpdateAnimatorParameters()
    {
        if ( ReferenceEquals( animator, null ) )
        {
            return;
        }

        base.UpdateAnimatorParameters();
    }

    private void UpdateCrouchState()
    {
        /// 필요할때만 코루틴으로 처리하는게 좋을듯
        if ( IsCrouching )
        {
            capsule.height = Mathf.MoveTowards( capsule.height, originCapsuleHeight * 0.6f, Time.deltaTime * 5.0f );
        }
        else
        {
            capsule.height = Mathf.MoveTowards( capsule.height, originCapsuleHeight, Time.deltaTime * 5.0f );
        }

        animator.gameObject.transform.localPosition = ( capsule.height * 0.5f * Vector3.down );
    }

    private void UpdateInteraction()
    {
        if ( !isLocal )
        {
            return;
        }

        // 타겟팅
        {
            Ray ray = new Ray( Camera.main.transform.position, Camera.main.transform.forward );
            RaycastHit hit;

            Actor target = null;
            if ( Physics.Raycast( ray, out hit, focusInfo.RayDistance, focusInfo.CullingLayer ) )
            {
                if ( hit.collider == null )
                {
                    Debug.LogError( "Hit collider is null." );
                    return;
                }

                target = hit.collider.GetComponentInParent<Actor>();
            }

            if ( focusInfo.Target != target  )
            {
                focusInfo.Target = target;
                OnChangeFocusTarget( focusInfo.Target );
            }
        }

        if ( Input.GetKeyDown( focusInfo.InteractionKey ) && focusInfo.Target != null )
        {
            IInteractable interactor = focusInfo.Target as IInteractable;
            if ( !ReferenceEquals( interactor, null ) )
            {
                interactor.Interaction( this );
                OnChangeFocusTarget( null ); /// Destroy시 타겟이 남아있어서 재설정
            }
        }
    }

    private void SendSyncCrouch( bool _isCrouching )
    {
        if ( !isLocal )
        {
            return;
        }

        Protocol.Both.SyncCrouch protocol;
        protocol.Serial = serial;
        protocol.IsCrouch = _isCrouching;
        Network.Instance.Send( protocol );
    }

    private void SendSyncGrounded( bool _isGrounded )
    {
        if ( !isLocal )
        {
            return;
        }

        Protocol.Both.SyncGrounded protocol;
        protocol.Serial = serial;
        protocol.IsGrounded = _isGrounded;
        Network.Instance.Send( protocol );
    }
}
