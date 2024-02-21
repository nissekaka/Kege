#include "deferred_common.hlsli"

struct PixelInput
{
    float3 worldPos : POSITION;
    float4 position : SV_POSITION;
    float2 texCoord : TEXCOORD;
    float3 normal : NORMAL;
    float3 worldNormal : WORLDNORMAL;
    float3 tangent : TANGENT;
    float3 bitan : BITANGENT;
};

//static int N_SAMPLES = 151;
//static float R_MAX = 0.01f; // Maximum sampling radius.
//static float RSM_INTENSITY = 100.0f;

//static float2 POISSON_DISC_151[151] =
//{
//    float2(-0.2105855460826458, 0.44847103832481405),
//float2(-0.3249367874595762, 0.45194211733208256),
//float2(-0.2215745524228665, 0.24943447347438874),
//float2(-0.060082623613624264, 0.28588622282467613),
//float2(-0.10876121836372676, 0.1516908621362929),
//float2(0.029608993270258344, 0.36225092203424447),
//float2(-0.4416077740215665, 0.5922474888127149),
//float2(0.09844974909210591, 0.5293232385931546),
//float2(-0.242750816128263, 0.5652476929932564),
//float2(0.1849719239883465, 0.42269268543818006),
//float2(0.3025579097363722, 0.5054990750455668),
//float2(-0.6198904961790854, 0.4923955674020746),
//float2(0.1371459445570682, 0.2256376247802212),
//float2(0.07335761833299381, 0.060664733131448045),
//float2(-0.0981114897551485, 0.5038558748024538),
//float2(0.02105320196048943, 0.20449635334174143),
//float2(-0.577615989084985, 0.6751017664470389),
//float2(-0.7047095788535609, 0.6703810066791474),
//float2(-0.3461478915533044, 0.2767411968699174),
//float2(0.22826800325817365, 0.6885961509356942),
//float2(0.008557608781530446, 0.6846674257651739),
//float2(-0.7457514972966308, 0.5525938412112139),
//float2(-0.12675177599317122, 0.03807380965906315),
//float2(0.26001862548878574, 0.9017228978398559),
//float2(-0.12749254424512912, 0.6394141870327132),
//float2(-0.25490982834883025, -0.009623207028237046),
//float2(-0.2832188417687387, 0.7080442976582488),
//float2(-0.5568853398254002, 0.8281675565698587),
//float2(-0.4502169909230217, 0.1983838328508294),
//float2(-0.3374755253275854, 0.12304984569933919),
//float2(0.28022304577413726, 0.19855055835921753),
//float2(-0.21036466694065437, 0.9159129231359),
//float2(0.18360415112566697, 0.02168652914349556),
//float2(-0.4584645969682012, 0.3302427234342433),
//float2(-0.4479511888159913, 0.7410125839426094),
//float2(0.3991624033194776, 0.7988858591043992),
//float2(-0.721662130539607, 0.39995242005235676),
//float2(-0.3442261159253708, 0.8599343768096765),
//float2(-0.8466679858222527, 0.36459222475755637),
//float2(-0.45878496236317035, 0.47334713275894513),
//float2(-0.4372446791568142, -0.0165798426817938),
//float2(0.02100915885650645, -0.06981332644045746),
//float2(0.13110358045707327, 0.8247787987084165),
//float2(0.00823456530315858, 0.8107451419490457),
//float2(0.42797771436580456, 0.5587115156373583),
//float2(0.32214857014394704, 0.33031482110713184),
//float2(-0.22137684848340067, 0.1136363932619775),
//float2(-0.14193395356177185, -0.08604938105381399),
//float2(-0.5536753158083249, -0.18073175280365916),
//float2(-0.5070399904334678, 0.08689622459455082),
//float2(0.52028114635073, 0.3812556209224749),
//float2(-0.08344310105240482, 0.95881586699393),
//float2(-0.5862241280594775, 0.31599501752459136),
//float2(-0.7424917104785005, -0.14495274618636356),
//float2(-0.71268208479312, 0.18074542096312163),
//float2(0.108990027583584, -0.15666655867133028),
//float2(-0.015166808630370787, -0.2487589023599489),
//float2(0.13234925413927323, 0.9492511038584823),
//float2(-0.12322536949396168, 0.7992904731603836),
//float2(0.36403836196961614, 0.6617387426658699),
//float2(0.39955450741769316, 0.41547955572951234),
//float2(-0.5664427084973017, 0.18679436589332576),
//float2(-0.6944101245020164, -0.01343990502836212),
//float2(0.5625324116044776, 0.5279964765578082),
//float2(0.3782464905914933, 0.12643160642731033),
//float2(-0.8454075321944396, 0.014131655121489839),
//float2(0.4206171451921148, 0.25475356609725885),
//float2(-0.9192606507897227, -0.1425594778539232),
//float2(-0.9654723633960183, 0.19512401454226325),
//float2(0.3301371414109351, -0.06358517986619083),
//float2(-0.16766134181208847, -0.2189845370715473),
//float2(-0.542947723738074, -0.061891341033254776),
//float2(0.3145600952238312, -0.18962449885503818),
//float2(-0.3877834273727072, -0.11963285486335329),
//float2(-0.6912736594144696, -0.2576592094385691),
//float2(0.5299377470642048, 0.6429034882840958),
//float2(-0.9092290171229506, -0.2576042816799101),
//float2(-0.756598574200095, -0.3783062261042447),
//float2(-0.3359282441771242, -0.22535842315392463),
//float2(-0.19113437626591812, -0.37544518075404265),
//float2(-0.8316639544686806, 0.24888528214021544),
//float2(-0.3842335024447435, -0.3707398001959058),
//float2(0.0833362819102259, -0.36313489626173856),
//float2(-0.04703622100738214, -0.36769015058577315),
//float2(0.492677762334105, 0.10573129785820412),
//float2(0.5145026631513785, 0.7862005812142474),
//float2(-0.0805949863665465, -0.5098442295614739),
//float2(-0.36144986019027736, -0.5221245642722397),
//float2(-0.5338520212539983, -0.41540245649168794),
//float2(0.08799899906907926, -0.5327065307354413),
//float2(0.6051010684289804, 0.03429146108464676),
//float2(-0.5199138995020798, -0.5290112928940491),
//float2(-0.21573024720399891, -0.5777225511384059),
//float2(-0.48401194020273963, -0.28195487161906563),
//float2(-0.2523937202004908, -0.13494585553274363),
//float2(0.6583622019076383, 0.25357468334574595),
//float2(0.7332514662646883, 0.5471373800529391),
//float2(0.1820533681249794, -0.25589649723524566),
//float2(0.4224076053791508, -0.29088682980126956),
//float2(0.7177903741554872, 0.07235396433442842),
//float2(-0.4771029660046049, -0.6719876518886342),
//float2(0.2534785901118639, -0.40897467057733083),
//float2(-0.27600712311713327, -0.680828645289126),
//float2(-0.6185492815793612, -0.7453116089045722),
//float2(-0.29124157302402065, -0.857368819598208),
//float2(0.6478925289192881, 0.6345225707327742),
//float2(0.5606766211201131, -0.16747590407909985),
//float2(-0.6544148295496951, -0.44189162385444347),
//float2(0.7660305835364689, -0.1163440033664408),
//float2(-0.984396930296487, -0.00434290617490074),
//float2(0.43218518472597256, -0.17091863644713534),
//float2(-0.04413960266224226, -0.703047548286027),
//float2(0.5362856037867374, -0.30318221730699),
//float2(0.8502947919017667, 0.1312310157856249),
//float2(-0.6479918401665218, -0.6051526163645965),
//float2(0.4240350465627807, 0.00822566312902584),
//float2(0.2230332514623785, -0.5431579662998082),
//float2(0.6464790776805429, 0.37436179966037253),
//float2(0.9224902048831116, 0.041580855014303086),
//float2(-0.4324019065826592, -0.787299608631407),
//float2(0.13872153774843388, -0.7113018553685244),
//float2(0.8531859057169597, 0.40581471690902804),
//float2(0.5453953127927622, 0.2093370569631816),
//float2(0.9243895369495867, 0.2642707871055614),
//float2(-0.006129765418066646, -0.8511686376433512),
//float2(0.27384668647208743, -0.7164307699873447),
//float2(0.20004837507761009, -0.8351498888499372),
//float2(0.7777056608159021, 0.2404310438231101),
//float2(-0.7484978798285156, -0.5396097698216022),
//float2(-0.8403766405915871, -0.4558144551108865),
//float2(0.3522383577039254, -0.4758041359031191),
//float2(0.783890871969178, -0.34279882440569154),
//float2(-0.18978613691531687, -0.784090953537453),
//float2(-0.09560238051589443, -0.9250374202209017),
//float2(0.048502964663310166, -0.9549042447300811),
//float2(0.6456628770495716, -0.33756595771091413),
//float2(0.9593865924353975, -0.12327064637742902),
//float2(0.6783583896965726, -0.4787960858888588),
//float2(0.405881740124189, -0.6279578719333552),
//float2(0.8224294731607125, -0.22993697489365883),
//float2(0.5268791482035144, -0.05272822133058308),
//float2(0.6580553825480155, -0.07591007732029698),
//float2(0.5299876015032885, -0.4972122527133582),
//float2(0.7208672506325395, -0.5952659848227987),
//float2(0.5432340182562456, -0.652824644026825),
//float2(0.6747160604935316, -0.19158382758078019),
//float2(0.2201129257859169, -0.1252318363959143),
//float2(0.7993354103716619, -0.46184850776665365),
//float2(0.6856655239366938, -0.7049805434613721),
//float2(0.3666048733591001, -0.803469722802703),
//float2(0.48271502610609507, -0.8050882593415389)
//};

//float3 IndirectLighting(float2 aUv, float3 aN, float3 aX)
//{
//    float3 rsmOutput = { 0.0f, 0.0f, 0.0f };

//    for (int i = 0; i < N_SAMPLES; i++)	// Sum contributions of sampling locations
//    {
//        float2 uv = aUv + R_MAX * POISSON_DISC_151[i];
//        float3 flux = RSM_FluxTex.Sample(defaultSampler, uv).rgb; // Collect components from corresponding RSM textures
//        float3 x_p = RSM_WorldPositionTex.Sample(defaultSampler, uv).xyz; // Position (x_p) and normal (n_p) are in world coordinates too
//        float3 n_p = RSM_NormalTex.Sample(defaultSampler, uv).xyz;

//		// Irradiance at current fragment w.r.t. pixel light at uv
//        float3 r = aX - x_p; // Difference vector
//        float d2 = dot(r, r); // Square distance
//        float3 E_p = flux * (max(0.0, dot(n_p, r)) * max(0.0, dot(aN, -r)));
//        E_p *= POISSON_DISC_151[i].x * POISSON_DISC_151[i].x / (d2 * d2); // Weighting contribution and normalizing

//        rsmOutput += E_p; // Accumulate
//    }

//    return RSM_INTENSITY * rsmOutput; // Modulate result with some intensity value.
//}

GBufferOutput main(PixelInput aInput)
{
    float4 albedo = colourTex.Sample(defaultSampler, aInput.texCoord).rgba;
    if (albedo.a < 0.5f)
    {
        discard;
    }

    float3 normal = float3(normalTex.Sample(defaultSampler, aInput.texCoord).rg, 1.0f);
    //float3 normal = normalTex.Sample(defaultSampler, scaledUV).wyz;
    //float ambientOcclusion = normal.z;
    normal = 2.0f * normal - 1.0f;
    normal.z = sqrt(1 - saturate(normal.x * normal.x + normal.y * normal.y));
    normal = normalize(normal);

    float3x3 TBN = float3x3(normalize(aInput.tangent.xyz),
                            normalize(-aInput.bitan.xyz),
                            normalize(aInput.normal.xyz));
    TBN = transpose(TBN);

    const float3 pixelNormal = normalize(mul(TBN, normal));

    float4 material = materialTex.Sample(defaultSampler, aInput.texCoord);
    float ambientOcclusion = material.r;

    GBufferOutput output;
    output.worldPosition = float4(aInput.worldPos, 1.0f);
    output.albedo = float4(albedo.rgb, 1.0f);
    output.normal = float4(0.5f + 0.5f * pixelNormal, 1.0f);
    output.material = float4(material.rgb, 1.0f);
    output.ambientOcclusionAndCustom = float4(ambientOcclusion, aInput.worldNormal); // gba are unused, put whatever data you want here!
    output.ambientOcclusionAndCustom.g = 0.0f;

    //output.rsm = RSM_NormalTex.Sample(defaultSampler, aInput.texCoord).rgba;
    //output.rsm = float4(IndirectLighting(aInput.texCoord, aInput.worldNormal, output.worldPosition), 1.0f);
    output.rsm = float4(0.0f, 0.0f, 0.0f, 1.0f);
    return output;
}

