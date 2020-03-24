using System.Collections;
using System.Collections.Generic;
using System.Runtime.InteropServices;
using UnityEngine;
using System.Linq;

public class FaceTracker : MonoBehaviour
{
    public static Vector2 facePosition, nosePosition, input;
    public static Vector2[] eyePositions;
    [Range(0,10)][SerializeField] private int _maxEyes;
    [SerializeField] private bool showCapture = false;
    private OpenCVCircle[] _eyes;
    private OpenCVCircle _faces, _nose;
    public bool active = false;
    private int camWidth, camHeight, minFaceSize = 200;
    // Start is called before the first frame update
    void Start()
    {
        ZeroInput();
        int camIndex;

        camIndex = camWidth = camHeight = 0;

        int result = OpenCVFace.Initialise(ref camIndex, ref camWidth, ref camHeight);

        Debug.Log(camWidth + ":" + camHeight);

        _eyes = new OpenCVCircle[_maxEyes];

        eyePositions = new Vector2[_maxEyes];

        if (result == 0)
            active = true;
    }

    private static void ZeroInput()
    {
        facePosition = nosePosition = input = new Vector2(0, 0);
        eyePositions = new Vector2[] { new Vector2(0, 0), new Vector2(0, 0) };
    }

    private void OnApplicationQuit()
    {
        if (active)
            OpenCVFace.Release();
    }

    void Update()
    {
        if(active)
        {
            DetectFrontFaceAndEyes();
        }
    }

    private void DetectFrontFaceAndEyes()
    {
        int result = 1;
        unsafe
        {
            fixed (OpenCVCircle* eyesDetected = _eyes)
            {
                //Run the openCV C++ code, returning the face position, eye positions and nose position
                result = OpenCVFace.DetectFace(ref _faces, minFaceSize, eyesDetected, _maxEyes, ref _nose, showCapture);
                //if (result == 0)
                //{
                //    OpenCVFace.DetectEyes(eyesDetected, _maxEyes);
                //    OpenCVFace.DetectNose(ref _nose);
                //}
            }
        }

        //Can switch case the output, eg -1 means no face detected etc. then can zero the player input or potentially pause if face is not detected
        if (result == 0)
        {
            facePosition = new Vector2(_faces.X, _faces.Y);            
            eyePositions = _eyes.Select(s => new Vector2((float)s.X / camWidth, (float)s.Y / camHeight)).ToArray();
            nosePosition = new Vector2(_nose.X, _nose.Y);

            input = facePosition - nosePosition;
            Debug.Log("Face X = " + facePosition.x + ": Face Y = " + facePosition.y);
            Debug.Log("Nose X = " + nosePosition.x + ": Nose Y = " + nosePosition.y);
            Debug.Log("Delta X = " + input.x + ": Delta Y = " + input.y);

        }
        else
        {
            ZeroInput();
        }
    }
}
internal static class OpenCVFace
{
    [DllImport("FaceInput")]
    internal static extern int Initialise(ref int camIndex, ref int camWidth, ref int camHeight);

    [DllImport("FaceInput")]
    internal unsafe static extern int DetectFace(ref OpenCVCircle facePos, int minFaceSize, OpenCVCircle* eyePositions, int maxEyes, ref OpenCVCircle nosePosition, bool showCap);

    //[DllImport("FaceInput")]
    //internal unsafe static extern int DetectEyes(OpenCVCircle* facePos, int maxEyes);

    //[DllImport("FaceInput")]
    //internal unsafe static extern int DetectNose(ref OpenCVCircle nosePos);

    [DllImport("FaceInput")]
    internal static extern void Release();
}

[StructLayout(LayoutKind.Sequential, Size = 96)]
public struct OpenCVCircle
{
    public int X, Y, Radius;
}
