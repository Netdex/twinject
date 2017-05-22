# Bullet Update Procedure

The following is the update routine used to step entity locations by their velocity:

`pos_update .text 00410A70 00000043 00000008 00000004 R . . . B . .`
```c
int __fastcall pos_update(int a1, int a2, int a3)
{
  *(float *)a1 = *(float *)a1 + *(float *)a3;
  *(float *)(a1 + 4) = *(float *)(a1 + 4) + *(float *)(a3 + 4);
  *(float *)(a1 + 8) = *(float *)(a1 + 8) + *(float *)(a3 + 8);
  return a1;
}

// inferred structure
struct vector {
  float unknown;
  float x;
  float y;
};

```
```
a1: addr of entity location vector
a2: who the hell knows
a3: addr of entity velocity vector
```

The following is the update routine for processing a single bullet:

```c
signed int __thiscall bullet_process(char *this)
{
  int v2; // edx@31
  
  ...
  
  char *v37; // [sp+64h] [bp-20h]@1
  
  ...

  v27 = this;
  v40 = 0;
  v37 = this + 108672;
  if ( ((unsigned int)dword_164D0B4 >> 10) & 1 )
    return 1;
  sub_440500(&unk_1653648);
  *((_DWORD *)v27 + 1763662) = 0;
  sub_4321B0(v27);
  for ( i = 0; i < 1536; ++i )
  {
    if ( *((_WORD *)v37 + 1756) )
    {
      ++*((_DWORD *)v27 + 1763662);
      switch ( *((_WORD *)v37 + 1756) )
      {
        case 1:
          goto bullet_update;
		  
        ...
		
bullet_update:
          sub_42FFC0(v37);
		  
          ...
		  
          v2 = (int)v37;
          if ( *((_DWORD *)v37 + 874) )
          {
            v2 = (int)v37;
            --*((_DWORD *)v37 + 874);
          }
          if ( !byte_160F534 )
            pos_update((int)(v37 + 3396), v2, (int)(v37 + 3408));		// << the important part
            
            ...
```
