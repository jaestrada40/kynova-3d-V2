# Publicar la página 3D en Coolify (Hostinger VPS)

La página es un solo `index.html` estático. No necesita base de datos ni variables de entorno.
Coolify la sirve con nginx usando el `Dockerfile` de esta carpeta.

## Lo que necesitas antes
- Coolify ya instalado y funcionando en el VPS.
- El repositorio de GitHub `jaestrada40/kynova-3d-V2` (ver el paso 2).
- Un dominio o subdominio, por ejemplo `kynova.tudominio.com`.

## 1. Apuntar el dominio al VPS
En el panel de DNS (Hostinger):

| Tipo | Nombre | Valor |
|------|--------|-------|
| A    | `kynova` (o el subdominio que quieras) | la IP pública del VPS |

Espera unos minutos a que propague. Se puede comprobar con `nslookup kynova.tudominio.com`.

## 2. Tener el código en GitHub
Esta página vive en el repositorio `https://github.com/jaestrada40/kynova-3d-V2`, con `index.html`, `Dockerfile`
y `nginx.conf` en la raíz. Para actualizarla: cambiar los archivos, hacer commit y `git push origin main`.

## 3. Crear el recurso en Coolify
1. **Projects → (tu proyecto) → New Resource.**
2. Elegir **Public Repository** (o **Private Repository (with GitHub App)** si el repo es privado).
3. Pegar la URL del repositorio y elegir la rama `main`.
4. En **Build Pack** elegir **Dockerfile**.
5. **Base Directory:** `/`
6. **Ports Exposes:** `80`.
7. En **Domains** escribir `https://kynova.tudominio.com`. Coolify pide el certificado HTTPS solo (Let's Encrypt).
8. (Opcional) En **Health Check** activar la ruta `/healthz`.
9. Pulsar **Deploy**.

Al terminar, abrir `https://kynova.tudominio.com`. Cada vez que se haga push a `main`, se puede volver a
desplegar con **Deploy**, o activar **Auto Deploy** con el webhook de GitHub.

## Si algo falla
| Síntoma | Causa probable |
|---------|----------------|
| `502` o `Bad Gateway` | Falta poner `80` en **Ports Exposes**. |
| El navegador dice que no es seguro | El dominio todavía no apunta a la IP del VPS. Revisar el registro A. |
| `Dockerfile not found` | **Base Directory** mal puesto. |
| Pantalla sin escena 3D | El navegador del visitante no tiene WebGL, o está sin internet (la página carga Three.js desde jsDelivr y las fuentes desde Google Fonts). |

## Nota sobre internet
Quien abra la página necesita conexión: Three.js se descarga de `cdn.jsdelivr.net` y las fuentes de
`fonts.googleapis.com`. Si se va a presentar en un lugar sin internet, hay que descargar esos archivos y
servirlos desde esta misma carpeta.
