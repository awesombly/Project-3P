﻿using System.Collections;
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
    private Dictionary<int/*index*/, Equipment> equipQuickslot = new Dictionary<int/*index*/, Equipment>();

    public Equipment testEquip;
    public Equipment testEquip2;

    protected override void Awake()
    {
        base.Awake();

        foreach ( BoneInfo info in boneList )
        {
            boneInfos.Add( info.BoneType, info );
        }

        Vector3 rootScale = boneInfos[ EBoneType.Root ].Reference.lossyScale;
        BoneInfo.OriginalScale = new Vector3( 1.0f / rootScale.x, 1.0f / rootScale.y, 1.0f / rootScale.z );
    }

    private void Start()
    {
        SetEquipQuickslot( 0, testEquip );
        SetEquipQuickslot( 1, testEquip2 );
        SetEquipQuickslot( 2, testEquip );
        SetEquipQuickslot( 3, testEquip2 );
        SetEquipQuickslot( 4, testEquip );
        SetEquipQuickslot( 5, testEquip2 );

        UseEquipQuickslot( 0 );
    }

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
}
