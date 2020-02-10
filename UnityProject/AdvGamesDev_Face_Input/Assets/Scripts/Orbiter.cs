using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class Orbiter : MonoBehaviour
{
    float range;
    [SerializeField] private float orbitSpeed = 1f;

    // Start is called before the first frame update
    void Start()
    {
        range = transform.localPosition.magnitude;
    }

    // Update is called once per frame
    void Update()
    {
        
    }
}
