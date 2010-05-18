#include <windows.h>
#include <stdio.h>

int main(int argc, char *argv[])
{
  printf("DlgResToDlgTemplate v1.0 (c) <lallousx86@yahoo.com>\n\n");

  if (argc < 4)
  {
    printf("usage: %s MODULE_FILE DLG_ID FILE.H\n", argv[0]);
    return -1;
  }

  int dlgID = atoi(argv[2]);

  HMODULE hModule;
  hModule = ::LoadLibraryEx(argv[1], 0, LOAD_LIBRARY_AS_DATAFILE);
  if (hModule == NULL)
  {
    printf("Could not load module '%s'\n", argv[1]);
    return -1;
  }

  HRSRC hrsrc = ::FindResource(hModule, (LPCTSTR) dlgID, RT_DIALOG);
  if (hrsrc == NULL)
  {
    printf("Cannot find dialog resource with id '%d'\n", dlgID);
    return -2;
  }

  HGLOBAL hglobalRsrc = ::LoadResource(hModule, hrsrc);
  if (hglobalRsrc == NULL)
  {
    printf("Cannot load resource!\n");
    return -3;
  }

  char *buf = (char *)::LockResource(hglobalRsrc);
  if (buf == NULL)
  {
    printf("Cannot lock resource!\n");
    return -4;
  }

  DWORD sz = ::SizeofResource(hModule, hrsrc);

  FILE *fp = fopen(argv[3], "wb");
  if (fp == NULL)
  {
    printf("Could not open '%s' for writing\n", argv[3]);
    return -5;
  }

  
  char temp[100];

  sprintf(temp, "static unsigned char dlg_%d[] = \n{\n\t", dlgID);
  fwrite(temp, 1, strlen(temp), fp);

  sz--;
  for (DWORD i=0;i<=sz;i++)
  {
    sprintf(temp, "0x%02x", (unsigned char)(buf[i] & 0xFF));
    fwrite(temp, 1, strlen(temp), fp);

    // don't append a comma if it was last byte
    if (i != sz)
    {
      fwrite(", ", 1, 1, fp);
    }

    if (i && (i % 16 == 0))
      fwrite("\n\t", 1, 2, fp);
  }

  strcpy(temp, "\n};\n");
  fwrite(temp, 1, strlen(temp), fp);

  fclose(fp);
  ::FreeLibrary(hModule);

  printf("generated %s (%d bytes) successfully!\n", argv[3], sz);

  return 0;
}