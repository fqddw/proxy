#include "HttpBody.h"
#include "CommonType.h"
Stream* HttpBody::ToStream(Stream* pStream)
{
		Stream* pLocalStream = new Stream();
		pLocalStream->Append(pStream->GetData(),pStream->GetLength());
		return pLocalStream;
}
int HttpBody::IsEnd()
{
		return TRUE;
}
