using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public enum EBoneType
{
    Root,
    LeftHand,
    RightHand,
}

public class Player : Character
{
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
    internal Dictionary<int/*index*/, Equipment> equipQuickslot = new Dictionary<int/*index*/, Equipment>();

    public List<Equipment> testEquips;

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


    protected override void Awake()
    {
        base.Awake();

        OnChangeCrouching += SendSyncCrouch;
        OnChangeGrounded += SendSyncGrounded;

        foreach ( BoneInfo info in boneList )
        {
            boneInfos.Add( info.BoneType, info );
        }

        Vector3 rootScale = boneInfos[ EBoneType.Root ].Reference.lossyScale;
        BoneInfo.OriginalScale = new Vector3( 1.0f / rootScale.x, 1.0f / rootScale.y, 1.0f / rootScale.z );
        
        for ( int i = 0; i < testEquips.Count; ++i )
        {
            SetEquipQuickslot( i, testEquips[ i ] );
        }

        UseEquipQuickslot( 0 );
    }

    protected override void FixedUpdate()
    {
        base.FixedUpdate();

        UpdateCrouchState();
    }

    #region Equipment
    public void SetEquipment( Equipment equip )
    {
        if ( equip == null )
        {
            Debug.LogWarning( "equip is null." );
            return;
        }

        ReleaseEquipment( equip.equipType );

        EquipInfo equipInfo;
        equipInfo.Equip = equip;
        equipInfo.Models = new List<GameObject>();

        foreach ( Equipment.ModelInfo info in equip.modelInfos )
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

        equipInfos.Add( equip.equipType, equipInfo );
    }

    public void ReleaseEquipment( EEquipType equipType )
    {
        if ( !equipInfos.ContainsKey( equipType ) )
        {
            return;
        }

        EquipInfo prevEquip = equipInfos[ equipType ];
        foreach ( GameObject model in prevEquip.Models )
        {
            Destroy( model );
        }

        equipInfos.Remove( equipType );
    }

    public void SetEquipQuickslot( int index, Equipment equip )
    {
        if ( !equipQuickslot.ContainsKey( index ) )
        {
            equipQuickslot.Add( index, null );
        }

        equipQuickslot[ index ] = equip;
    }

    public void UseEquipQuickslot( int index )
    {
        if ( !equipQuickslot.ContainsKey( index ) )
        {
            return;
        }

        SetEquipment( equipQuickslot[ index ] );
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
        base.UpdateAnimatorParameters();

        if ( ReferenceEquals( animator, null ) )
        {
            return;
        }

        animator.SetBool( AnimatorParameters.IsStrafing, true );
    }

    private void UpdateCrouchState()
    {
        /// 필요할때만 코루틴으로 처리하는게 좋을듯
        if ( IsCrouching )
        {
            capsule.height = Mathf.MoveTowards( capsule.height, originCapsuleHeight * 0.65f, Time.deltaTime * 5.0f );
        }
        else
        {
            capsule.height = Mathf.MoveTowards( capsule.height, originCapsuleHeight, Time.deltaTime * 5.0f );
        }

        animator.gameObject.transform.localPosition = ( capsule.height * 0.5f * Vector3.down );
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
