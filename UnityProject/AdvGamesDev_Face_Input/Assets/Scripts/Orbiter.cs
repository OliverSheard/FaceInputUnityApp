using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using System;
public class Orbiter : MonoBehaviour
{    
    [SerializeField] private Vector3 orbitSpeed = new Vector3(1,1,1);
    [SerializeField] private bool xRotation, yRotation, zRotation;
    private Quaternion baseRotation;

    // Start is called before the first frame update
    void Start()
    {
        baseRotation = transform.rotation;
    }

    // Update is called once per frame
    void Update()
    {
        var x = xRotation ? orbitSpeed.x * Time.deltaTime : 0;
        var y = yRotation ? orbitSpeed.y * Time.deltaTime : 0;
        var z = zRotation ? orbitSpeed.z * Time.deltaTime : 0;

        transform.Rotate(x, y, z);
    }
}
