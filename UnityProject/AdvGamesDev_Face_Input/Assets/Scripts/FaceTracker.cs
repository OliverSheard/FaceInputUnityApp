using System.Collections;
using System.Collections.Generic;
using System.Runtime.InteropServices;
using UnityEngine;
using System.Linq;

public class FaceTracker : MonoBehaviour
{
    public static Vector2 facePosition;
    public static Vector2[] eyePositions;
    [Range(0,10)][SerializeField] private int _maxEyes;
    int _maxFaces = 1;
    [SerializeField] private bool showCapture = false;
    private OpenCVCircle[] _eyes;
    private OpenCVCircle _faces;
    private bool active = false;
    // Start is called before the first frame update
    void Start()
    {
        int camIndex, camWidth, camHeight;

        camIndex = camWidth = camHeight = 0;

        int result = OpenCVFace.Initialise(ref camIndex,ref camWidth,ref camHeight);

        Debug.Log(result);

        _eyes = new OpenCVCircle[_maxEyes];

        eyePositions = new Vector2[_maxEyes];

        if(result == 0)
            active = true;
    }

    private void OnApplicationQuit()
    {
        if (active)
            OpenCVFace.Release();
    }

    // Update is called once per frame
    void Update()
    {
        if(active)
        {
            int result = 1;
            unsafe
            {
                fixed(OpenCVCircle* eyesDetected = _eyes)
                {
                    result = OpenCVFace.DetectFace(_faces, eyesDetected, _maxEyes, _maxFaces, showCapture);
                    Debug.Log(eyesDetected[0].X);

                    
                    //fixed (OpenCVCircle* faces = _faces)
                    //{
                        
                    //}
                    
                }
            }

            if(result == 0)
            {
                Debug.Log("2: " + _faces.X + ":" + _faces.Y);
                Debug.Log(_eyes[0].X);

                facePosition = new Vector2(_faces.X, _faces.Y);
            }                       
        }
    }
}
internal static class OpenCVFace
{
    [DllImport("FaceInput")]
    internal static extern int Initialise(ref int camIndex, ref int camWidth, ref int camHeight);

    [DllImport("FaceInput")]
    internal unsafe static extern int DetectFace(OpenCVCircle facePos, OpenCVCircle* eyes, int maxEyes, int maxFaces, bool showCap);

    [DllImport("FaceInput")]
    internal static extern void Release();
}

[StructLayout(LayoutKind.Sequential, Size = 96)]
public struct OpenCVCircle
{
    public int X, Y, Radius;
}

[StructLayout(LayoutKind.Sequential, Size = 36)]
public struct Face
{
    OpenCVCircle facePosition;
    OpenCVCircle[] eyePositions;
}
