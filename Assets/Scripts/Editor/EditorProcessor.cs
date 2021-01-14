using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEditor;
using UnityEditor.Callbacks;


public class EditorProcessor
{
    [PostProcessBuild( callbackOrder: 1 )]
    public static void OnPostprocessBuild( BuildTarget target, string pathToBuiltProject )
    {
        /// 에디터 빌드후 생성되는 잉여 파일
        string metaPath = "Assets/StreamingAssets/aa.meta";
        if ( System.IO.File.Exists( metaPath ) )
        {
            System.IO.File.Delete( metaPath );
        }
    }
}
