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
    public List<BoneInfo> boneList; // 인스펙터 편집용
    private Dictionary<EBoneType, BoneInfo> boneInfos;

    public GameObject testPrefab;

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
        {
            GameObject instance = Instantiate( testPrefab, boneInfos[ EBoneType.LeftHand ].Reference );
            instance.transform.localScale = BoneInfo.OriginalScale;
        }

        {
            GameObject instance = Instantiate( testPrefab, boneInfos[ EBoneType.RightHand ].Reference );
            instance.transform.localScale = BoneInfo.OriginalScale;
        }
    }
}
