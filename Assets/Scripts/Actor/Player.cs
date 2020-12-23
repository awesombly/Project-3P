using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class Player : Character
{
    [System.Serializable]
    public struct BoneInfo
    {
        // 자식 추가시 원본 크기 설정용
        static public Vector3 OriginalScale;

        public Transform Reference;
        public Transform LeftHand;
        public Transform RightHand;
    }
    public BoneInfo boneInfo;

    public GameObject testPrefab;

    protected override void Awake()
    {
        base.Awake();
        {
            Vector3 scale = boneInfo.Reference.lossyScale;
            BoneInfo.OriginalScale = new Vector3( 1.0f / scale.x, 1.0f / scale.y, 1.0f / scale.z );
        }
    }

    private void Start()
    {
        {
            GameObject instance = Instantiate( testPrefab, boneInfo.LeftHand );
            instance.transform.localScale = BoneInfo.OriginalScale;
        }

        {
            GameObject instance = Instantiate( testPrefab, boneInfo.RightHand );
            instance.transform.localScale = BoneInfo.OriginalScale;
        }
    }
}
