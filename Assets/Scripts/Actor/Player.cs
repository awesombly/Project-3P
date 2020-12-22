using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class Player : Character
{
    [System.Serializable]
    public struct BoneInfo
    {
        public Transform LeftHand;
        public Transform RightHand;
    }
    public BoneInfo boneInfo;

    public GameObject testPrefab;

    private void Start()
    {
        {
            GameObject instance = Instantiate( testPrefab );
            instance.transform.SetParent( boneInfo.LeftHand );
            instance.transform.localPosition = Vector3.zero;
        }

        {
            GameObject instance = Instantiate( testPrefab );
            instance.transform.SetParent( boneInfo.RightHand );
            instance.transform.localPosition = Vector3.zero;
        }
    }
}
